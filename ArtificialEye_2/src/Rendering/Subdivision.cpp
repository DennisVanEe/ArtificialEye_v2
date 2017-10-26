#include "Subdivision.hpp"

#include <unordered_map>

void ee::catmullClarkSubdiv(const Mesh* original, Mesh* subdivided)
{
    const VertBuffer& oVertices = original->getVerticesData();
    const MeshFaceBuffer& oFaces = original->getMeshFaceData();

    VertBuffer nVertices(oVertices);

    int startOfNewVertices = nVertices.size();

    // stores a list for each edge for faster iteration later (and becasue I am too lazzy to go crazy in this regard)
    std::unordered_map<std::pair<GLuint, GLuint>, std::vector<int>> edgePointsTable;

    struct FacePEdgeP
    {
        int m_facePoint;
        
        struct EdgePP
        {
            int m_edgePoint; // the edge point
            int m_points[2]; // the edge it is associated with

            EdgePP() : m_edgePoint(-1), m_points{-1, -1} {}
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

        // assign all the edges of that one face:
        for (int i = 0; i < 3; i++)
        {
            FacePEdgeP facePEdgeP;
            facePEdgeP.m_facePoint = nVertices.size() - 1;
            facePEdgeP.m_edgePoints[0].m_points[0] = face.m_indices[0];
            facePEdgeP.m_edgePoints[0].m_points[1] = face.m_indices[1];

            facePEdgeP.m_edgePoints[1].m_points[0] = face.m_indices[1];
            facePEdgeP.m_edgePoints[1].m_points[1] = face.m_indices[2];

            facePEdgeP.m_edgePoints[2].m_points[0] = face.m_indices[2];
            facePEdgeP.m_edgePoints[2].m_points[1] = face.m_indices[0];
            
            facePointPool[faceID] = facePEdgeP;
            vertexFaces[face.m_indices[0]].push_back(faceID); // denote what face belongs to this vertex
            vertexFaces[face.m_indices[1]].push_back(faceID);
            vertexFaces[face.m_indices[2]].push_back(faceID);
        }

        // assign edges to each of the points:
        std::pair<GLuint, GLuint> edges[3];
        edges[0] = std::make_pair(face.m_indices[0], face.m_indices[1]);
        edges[1] = std::make_pair(face.m_indices[1], face.m_indices[2]);
        edges[2] = std::make_pair(face.m_indices[2], face.m_indices[0]);

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

    // iterate over each facepoint (basically, like faces, but a little different):
    for (const auto& facePoint : facePointPool)
    {
        // first update both edgePoints:
        auto edgePoint0 = facePoint.first.first;
        auto edgePoint1 = facePoint.first.second;
        // the average of all points (edgePoint)
        glm::vec3 edgePoint = (nVertices[facePoint.second[0]].m_position + nVertices[facePoint.second[1]].m_position
            + nVertices[edge0].m_position + nVertices[edge1].m_position) * 0.25f;

        for (const auto& faceID : facePoint.second)
        {


            //const MeshFace& face = oFaces[faceID.first]; 
        }
    }

    // calculate the edge points
    for (int faceID = 0; faceID < oFaces.size(); faceID++)
    {
        const MeshFace& face = oFaces[faceID];

        // load all the edges and their corresponding faces:
        GLuint endPoints[3][2] =
        {
            {face.m_indices[0], face.m_indices[1]},
            {face.m_indices[1], face.m_indices[2]},
            {face.m_indices[2], face.m_indices[0]}
        };
        std::vector<std::pair<int, GLuint>> faces[3];
        for (int i = 0; i < 3; i++)
        {
            faces[i] = edgePointsTable.at(std::make_pair(endPoints[i][0], endPoints[i][1]));
        }

        // for each edge:
        for (int i = 0; i < 3; i++)
        {
            // the average of all points (edgePoint)
            glm::vec3 edgePoint = (nVertices[faces[i][0].second].m_position + nVertices[faces[i][1].second].m_position
                + nVertices[endPoints[i][0]].m_position + nVertices[endPoints[i][1]].m_position) * 0.25f;

            // now we have two new points:
            GLuint facePoint = faces[i][0].first == faceID ? faces[i][0].second : faces[i][1].second;
            GLuint 
        }
    }
}