#include "Subdivision.hpp"

#include <unordered_map>
#include <set>

struct edgePairHash 
{
    std::size_t operator() (const std::pair<GLuint, GLuint>& p) const 
    {
        std::size_t p0 = p.first;
        std::size_t p1 = p.second;
        return p0 | p1 << 32;
    }
};

std::pair<GLuint, GLuint> sortEdgePair(const std::pair<GLuint, GLuint>& p)
{
    GLuint min = glm::min(p.first, p.second);
    GLuint max = glm::max(p.first, p.second);
    return std::make_pair(min, max);
}

// This uses the catmull-clark subdivision algorithm to subdivide a mesh.
void ee::catmullClarkSubdiv(const VertBuffer& oVertices, const MeshFaceBuffer& oFaces,
    VertBuffer& nVertices, MeshFaceBuffer& nFaces)
{
    nVertices = oVertices;

    // table of edges and faces
    std::unordered_map<std::pair<GLuint, GLuint>, std::vector<int>, edgePairHash> edgePointsTable;

    struct FacePEdgeP
    {
        int m_facePoint;
        
        struct EdgePP
        {
            int m_edgePoint; // the edge point
            std::pair<GLuint, GLuint> m_points; // the edge it is associated with

            EdgePP() : m_edgePoint(-1), m_points(std::numeric_limits<GLuint>::max() , std::numeric_limits<GLuint>::max() ){}
        } m_edgePoints[3]; // always associated with 3 because there are three edges per face (it's a triangle)

        FacePEdgeP() : m_facePoint(-1) {}
    };
    std::vector<FacePEdgeP> facePointPool;
    facePointPool.resize(oFaces.size()); // the number of faces that we will use

    // these points index into the face edge pool
    std::vector<std::vector<int>> vertexFaces;
    vertexFaces.resize(nVertices.size());
   
    // assign all of the facePoints:
    for (int faceID = 0; faceID < oFaces.size(); faceID++)
    {
        const MeshFace& face = oFaces[faceID];

        // calculate the facePoint:
        glm::vec3 facePoint = (oVertices[face.m_indices[0]].m_position +
            oVertices[face.m_indices[1]].m_position +
            oVertices[face.m_indices[2]].m_position) * (1.f / 3);
        nVertices.push_back(facePoint); // add it to the mster list of vertices

        FacePEdgeP facePEdgeP;
        facePEdgeP.m_facePoint = nVertices.size() - 1;
        facePEdgeP.m_edgePoints[0].m_points = sortEdgePair(std::make_pair(face.m_indices[0], face.m_indices[1]));
        facePEdgeP.m_edgePoints[1].m_points = sortEdgePair(std::make_pair(face.m_indices[1], face.m_indices[2]));
        facePEdgeP.m_edgePoints[2].m_points = sortEdgePair(std::make_pair(face.m_indices[2], face.m_indices[0]));
            
        facePointPool[faceID] = facePEdgeP;
        vertexFaces[face.m_indices[0]].push_back(faceID); // denote what face belongs to this vertex
        vertexFaces[face.m_indices[1]].push_back(faceID);
        vertexFaces[face.m_indices[2]].push_back(faceID);

        // assign edges to each of the points:
        std::pair<GLuint, GLuint> edges[3];
        for (int i = 0; i < 3; i++)
        {
            edges[i] = facePEdgeP.m_edgePoints[i].m_points;
        }

        // associate them with edges
        for (int i = 0; i < 3; i++)
        {
            // check if the edge is already associated:
            if (edgePointsTable.find(edges[i]) == edgePointsTable.end())
            {
                // add the facepoint to the list (by creating a new one first)
                edgePointsTable.insert(std::make_pair(edges[i], std::vector<int>({faceID})));
            }
            // if so, add the new face (which is the face itself, not to be mistaken with 
            // the point
            else
            {
                edgePointsTable.at(edges[i]).push_back(faceID);
            }
        }
    }

    // assign all the edge points
    for (const auto& edges : edgePointsTable)
    {
        FacePEdgeP faces[2] = {facePointPool[edges.second[0]], facePointPool[edges.second[1]]};
        const std::pair<GLuint, GLuint>& endPoints = edges.first;

        glm::vec3 edgePoint = (nVertices[faces[0].m_facePoint].m_position + nVertices[faces[1].m_facePoint].m_position
            + nVertices[endPoints.first].m_position + nVertices[endPoints.second].m_position) * 0.25f;
        nVertices.push_back(edgePoint);

        int minP_edge = glm::min(endPoints.first, endPoints.second);
        int maxP_edge = glm::max(endPoints.first, endPoints.second);
        
        // loop through and update the vertices
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < 3; i++)
            {
                int minP_face = glm::min(faces[j].m_edgePoints[i].m_points[0],
                    faces[j].m_edgePoints[i].m_points[1]);
                int maxP_face = glm::max(faces[j].m_edgePoints[i].m_points[0],
                    faces[j].m_edgePoints[i].m_points[1]);

                // this is the correct edge:
                if (minP_edge == minP_face && maxP_edge == maxP_face)
                {
                    faces[j].m_edgePoints[i].m_edgePoint = nVertices.size() - 1;
                    break;
                }
            }
        }
    }

    // calculate the new points and do everything else, I guess
    for (int oldPointID = 0; oldPointID < vertexFaces.size(); oldPointID++)
    {
        const std::vector<int>& faceIDs = vertexFaces[oldPointID];
        glm::vec3 oldPoint = nVertices[oldPointID].m_position;

        glm::vec3 aveFacePoint, aveEdgePoint;
        std::set<int> edgePoints;
        for (int faceID : faceIDs)
        {
            aveFacePoint += nVertices[facePointPool[faceID].m_facePoint].m_position;
            edgePoints.insert(facePointPool[faceID].m_edgePoints[0].m_edgePoint);
            edgePoints.insert(facePointPool[faceID].m_edgePoints[1].m_edgePoint);
        }
        for (int edgePointID : edgePoints)
        {
            aveEdgePoint += nVertices[edgePointID].m_position;
        }
        aveFacePoint = aveFacePoint * (1.f / faceIDs.size());
        aveEdgePoint = aveEdgePoint * (1.f / edgePoints.size());

        float n = faceIDs.size();
        float m1 = (n - 3) / n;
        float m2 = 1 / n;
        float m3 = 2 / n;

        glm::vec3 newCoord = (m1 * oldPoint)
            + (m2 * aveFacePoint)
            + (m3 * aveEdgePoint);

        // update the new vertex as follows:
        nVertices[oldPointID] = newCoord;

        // add two meshes for each vertex:
        for (int faceID : faceIDs)
        {
            MeshFace face0, face1;
            face0.m_indices[0] = oldPointID, face0.m_indices[1] = facePointPool[faceID].m_facePoint,
                face0.m_indices[2] = facePointPool[faceID].m_edgePoints[0].m_edgePoint;
            face1.m_indices[0] = oldPointID, face0.m_indices[1] = facePointPool[faceID].m_facePoint,
                face0.m_indices[2] = facePointPool[faceID].m_edgePoints[1].m_edgePoint;
            nFaces.push_back(face0);
            nFaces.push_back(face1);
        }
    }

    nVertices = std::move(nVertices);
    nFaces = std::move(nFaces);
}