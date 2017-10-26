#include "Subdivision.hpp"

#include <unordered_map>
#include <set>

const GLuint GLUINT_MAX = std::numeric_limits<GLuint>::max();

class Edge
{
public:
    GLuint p0() const { return m_p0; }
    GLuint p1() const { return m_p1; }

    void set(GLuint p0, GLuint p1) { sort(); }

    bool operator==(const Edge& e) const { return m_p0 == e.m_p0 && m_p1 == e.m_p1; }
       
    Edge() : m_p0(GLUINT_MAX), m_p1(GLUINT_MAX) {}
    Edge(GLuint p0, GLuint p1) : m_p0(p0), m_p1(p1) { sort(); }

private:
    void sort()
    {
        GLuint p0 = glm::min(m_p0, m_p1);
        GLuint p1 = glm::max(m_p0, m_p1);
        m_p0 = p0, m_p1 = p1;
    }

    GLuint m_p0;
    GLuint m_p1;
};

struct edgePairHash 
{
    std::size_t operator() (const Edge& p) const
    {
        std::size_t p0 = static_cast<std::size_t>(p.p0());
        std::size_t p1 = static_cast<std::size_t>(p.p1());
        return p0 | p1 << 32;
    }
};

// This uses the catmull-clark subdivision algorithm to subdivide a mesh.
void ee::catmullClarkSubdiv(const VertBuffer& oVertices, const MeshFaceBuffer& oFaces,
    VertBuffer& nVertices, MeshFaceBuffer& nFaces)
{
    nVertices = oVertices;

    // table of edges and faces
    std::unordered_map<Edge, std::vector<GLuint>, edgePairHash> edgePointsTable;

    struct FacePEdgeP
    {
        GLuint m_facePoint;
        
        struct EdgePP
        {
            GLuint m_edgePoint; // the edge point
            Edge m_edge; // the edge it is associated with

            EdgePP() : m_edgePoint(GLUINT_MAX) {}
        } m_edgePoints[3]; // always associated with 3 because there are three edges per face (it's a triangle)

        FacePEdgeP() : m_facePoint(GLUINT_MAX) {}
    };
    std::vector<FacePEdgeP> facePointPool;
    facePointPool.resize(oFaces.size()); // the number of faces that we will use

    // these points index into the face edge pool
    std::vector<std::vector<GLuint>> vertexFaces;
    vertexFaces.resize(nVertices.size());
   
    // assign all of the facePoints:
    for (GLuint faceID = 0; faceID < oFaces.size(); faceID++)
    {
        const MeshFace& face = oFaces[faceID];

        // calculate the facePoint:
        glm::vec3 facePoint = (oVertices[face.m_indices[0]].m_position +
            oVertices[face.m_indices[1]].m_position +
            oVertices[face.m_indices[2]].m_position) * (1.f / 3);
        nVertices.push_back(facePoint); // add it to the mster list of vertices

        FacePEdgeP facePEdgeP;
        facePEdgeP.m_facePoint = nVertices.size() - 1;
        facePEdgeP.m_edgePoints[0].m_edge = Edge(face.m_indices[0], face.m_indices[1]);
        facePEdgeP.m_edgePoints[1].m_edge = Edge(face.m_indices[1], face.m_indices[2]);
        facePEdgeP.m_edgePoints[2].m_edge = Edge(face.m_indices[2], face.m_indices[0]);
            
        // associate each vertex with another face
        facePointPool[faceID] = facePEdgeP;
        vertexFaces[face.m_indices[0]].push_back(faceID);
        vertexFaces[face.m_indices[1]].push_back(faceID);
        vertexFaces[face.m_indices[2]].push_back(faceID);

        Edge edges[3] = 
        {
            facePEdgeP.m_edgePoints[0].m_edge,
            facePEdgeP.m_edgePoints[1].m_edge,
            facePEdgeP.m_edgePoints[2].m_edge
        };

        // set the faces for each edge (so, each edge gets 2 faces)
        for (int i = 0; i < 3; i++)
        {
            // check if the edge is already associated:
            if (edgePointsTable.find(edges[i]) == edgePointsTable.end())
            {
                // add the facepoint to the list (by creating a new one first)
                edgePointsTable.insert(std::make_pair(edges[i], std::vector<GLuint>({faceID})));
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
    int count = 0;
    for (const auto& edges : edgePointsTable)
    {
        FacePEdgeP* faces[2] = {&facePointPool[edges.second[0]], &facePointPool[edges.second[1]]};
        Edge currEdge = edges.first;
        glm::vec3 midEdge = (nVertices[currEdge.p0()].m_position + nVertices[currEdge.p1()].m_position) * 0.5f;
        glm::vec3 midFace = (nVertices[faces[0]->m_facePoint].m_position + nVertices[faces[1]->m_facePoint].m_position) * 0.5f;
        glm::vec3 edgePoint = (midEdge + midFace) * 0.5f;
        nVertices.push_back(edgePoint);

        // loop through and update the edgepoint as appropriate
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < 3; i++)
            {
                FacePEdgeP& currFace = *faces[j];

                // this is the correct edge:
                if (currEdge == currFace.m_edgePoints[i].m_edge)
                {
                    // now calculate the edge point:
                    assert(currFace.m_edgePoints[i].m_edgePoint == GLUINT_MAX);
                    currFace.m_edgePoints[i].m_edgePoint = nVertices.size() - 1;
                    break;
                }
            }
        }
    }

    // calculate the new points and do everything else, I guess
    for (GLuint oldPointID = 0; oldPointID < vertexFaces.size(); oldPointID++)
    {
        const std::vector<GLuint>& faceIDs = vertexFaces[oldPointID];
        glm::vec3 oldPoint = nVertices[oldPointID].m_position;

        glm::vec3 aveFacePoint, aveEdgePoint;
        std::set<GLuint> edgePoints;
        for (GLuint faceID : faceIDs)
        {
            aveFacePoint += nVertices[facePointPool[faceID].m_facePoint].m_position;
            edgePoints.insert(facePointPool[faceID].m_edgePoints[0].m_edgePoint);
            edgePoints.insert(facePointPool[faceID].m_edgePoints[1].m_edgePoint);
            edgePoints.insert(facePointPool[faceID].m_edgePoints[2].m_edgePoint);
        }
        for (GLuint edgePointID : edgePoints)
        {
            aveEdgePoint += nVertices[edgePointID].m_position;
        }
        aveFacePoint = aveFacePoint * (1.f / faceIDs.size());
        aveEdgePoint = aveEdgePoint * (1.f / edgePoints.size());

        float n = faceIDs.size();
        float m1 = (n - 3.f) / n;
        float m2 = 1.f / n;
        float m3 = 2.f / n;

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