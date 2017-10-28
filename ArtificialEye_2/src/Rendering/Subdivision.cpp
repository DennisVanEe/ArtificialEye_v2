#include "Subdivision.hpp"

#include <unordered_map>
#include <array>
#include <set>

const GLuint GLUINT_MAX = std::numeric_limits<GLuint>::max();

class Edge
{
public:
    int p0() const { return m_p0; }
    int p1() const { return m_p1; }

    int m_edgePoint;
    std::array<int, 2> m_faces; // doesn't support holes for now

    void set(GLuint p0, GLuint p1) { sort(); }

    bool operator==(const Edge& e) const { return m_p0 == e.m_p0 && m_p1 == e.m_p1; }
    bool operator<(const Edge& e) const { return hash() < e.hash(); }
       
    std::size_t hash() const
    {
        std::size_t p0 = static_cast<std::size_t>(m_p0);
        std::size_t p1 = static_cast<std::size_t>(m_p1);
        return p0 | p1 << 32; // simple combination hash
    }

    Edge() : m_p0(-1), m_p1(-1), m_edgePoint(-1), m_faces({-1, -1 }) {}
    Edge(GLuint p0, GLuint p1) : m_p0(p0), m_p1(p1), m_edgePoint(-1), m_faces({-1, -1}) { sort(); }

private:
    void sort()
    {
        int p0 = glm::min(m_p0, m_p1);
        int p1 = glm::max(m_p0, m_p1);
        m_p0 = p0, m_p1 = p1;
    }

    int m_p0;
    int m_p1;
};

struct EdgePairHash 
{
    std::size_t operator() (const Edge& p) const
    {
        return p.hash();
    }
};

struct EdgePair
{
    std::vector<int> m_edgePairs;
    int m_edgePoint = -1;
};

bool pointBelongsEdge(int point, Edge edge)
{
    return point == edge.p0() || point == edge.p1();
}

void lsdAddToEdgeList(int p0, int p1, int op, std::unordered_map<Edge, EdgePair, EdgePairHash>& edgeList)
{
    Edge edge(p0, p1);
    auto it = edgeList.find(edge);
    if (it == edgeList.end()) // if it doesn't exist, insert it
    {
        EdgePair pair;
        pair.m_edgePairs.push_back(op);
        auto insPair = std::make_pair(edge, pair);
        edgeList.insert(insPair);
    }
    else
    {
        it->second.m_edgePairs.push_back(op); // if it does, good
    }
}

int lsdCalcEdgeList(Edge edge, std::unordered_map<Edge, EdgePair, EdgePairHash>& edgeList, const ee::VertBuffer& oldVerts, ee::VertBuffer& vertices)
{
    auto it = edgeList.find(edge); assert(it != edgeList.end());
    if (it->second.m_edgePoint >= 0)
    {
        return it->second.m_edgePoint;
    }

    const std::vector<int>& pairs = it->second.m_edgePairs;
    glm::vec3 edgePoint = 3.f / 8.f * (oldVerts[edge.p0()].m_position + oldVerts[edge.p1()].m_position) +
        1.f / 8.f * (oldVerts[pairs[0]].m_position + oldVerts[pairs[1]].m_position);

    vertices.push_back(ee::Vertex(edgePoint));
    it->second.m_edgePoint = vertices.size() - 1;
    return it->second.m_edgePoint;
}

float lsdBetaConst(int n)
{
    if (n <= 3)
    {
        return 3.f / 16.f;
    }

    float inner = 3.f / 8.f + 1.f / 4.f * std::cos(ee::PI2 / n);
    return (1.f / n) * (5.f / 8.f - inner * inner);
}

void ee::loopSubdiv(const VertBuffer& oVertices, const MeshFaceBuffer& oFaces, VertBuffer& nVertices, MeshFaceBuffer& nFaces, unsigned recursion)
{
    if (recursion <= 0)
    {
        nVertices = oVertices;
        nFaces = oFaces;
        return;
    }

    std::unordered_map<Edge, EdgePair, EdgePairHash> edgeList; // stores edges and corresponding "pairs"
    std::vector<std::set<int>> vertexList; // stores neighboring points of points
    vertexList.resize(oVertices.size());

    // set up both lists:
    for (int faceID = 0; faceID < oFaces.size(); faceID++)
    {
        // prepare the edge faces:
        MeshFace f = oFaces[faceID];
        lsdAddToEdgeList(f.m_indices[0], f.m_indices[1], f.m_indices[2], edgeList);
        lsdAddToEdgeList(f.m_indices[1], f.m_indices[2], f.m_indices[0], edgeList);
        lsdAddToEdgeList(f.m_indices[2], f.m_indices[0], f.m_indices[1], edgeList);

        // prepare the vertexList:
        vertexList[f.m_indices[0]].insert(f.m_indices[1]); vertexList[f.m_indices[0]].insert(f.m_indices[2]);
        vertexList[f.m_indices[1]].insert(f.m_indices[0]); vertexList[f.m_indices[1]].insert(f.m_indices[2]);
        vertexList[f.m_indices[2]].insert(f.m_indices[1]); vertexList[f.m_indices[2]].insert(f.m_indices[0]);
    }

    // now we calculate the new points and the new faces
    VertBuffer tempVerts;
    tempVerts.resize(oVertices.size());
    MeshFaceBuffer tempFaces;

    for (MeshFace f : oFaces)
    {
        // some syntactical sugar:
        Edge edges[3] =
        {
            Edge(f.m_indices[0], f.m_indices[1]),
            Edge(f.m_indices[1], f.m_indices[2]),
            Edge(f.m_indices[2], f.m_indices[0])
        };

        // calculate the edge points for this face:
        int edgePoints[3];
        for (int i = 0; i < 3; i++) edgePoints[i] = lsdCalcEdgeList(edges[i], edgeList, oVertices, tempVerts);

        MeshFace middleFace;
        middleFace.m_indices[0] = edgePoints[0];
        glm::vec3 p0 = tempVerts[edgePoints[0]].m_position;
        middleFace.m_indices[1] = edgePoints[1];
        glm::vec3 p1 = tempVerts[edgePoints[0]].m_position;
        middleFace.m_indices[2] = edgePoints[2];
        glm::vec3 p2 = tempVerts[edgePoints[0]].m_position;
        tempFaces.push_back(middleFace); // we can do the middle one right now

        // for each vertex of the point:
        for (int i = 0; i < 3; i++)
        {
            int vertID = f.m_indices[i];

            // calculate the new vertex:
            if (!vertexList[vertID].empty())
            {
                glm::vec3 sumPoint, oldPoint = oVertices[vertID].m_position;
                for (int p : vertexList[vertID])
                {
                    sumPoint += oVertices[p].m_position;
                }

                int n = vertexList[vertID].size();
                float beta = lsdBetaConst(n);

                glm::vec3 newPoint = (1 - beta * n) * oldPoint + beta * sumPoint;
                tempVerts[vertID] = newPoint;
                vertexList[vertID].clear(); // indicate that we are done
            }

            std::vector<int> belongingEdgePoints;

            // select the appropriate edge points:
            for (int j = 0; j < 3; j++)
            {
                if (pointBelongsEdge(vertID, edges[j]))
                {
                    belongingEdgePoints.push_back(edgePoints[j]);
                }
            }
            assert(belongingEdgePoints.size() == 2);

            // push another triangle
            MeshFace currNewFace;
            currNewFace.m_indices[0] = belongingEdgePoints[0];
            currNewFace.m_indices[1] = belongingEdgePoints[1];
            currNewFace.m_indices[2] = vertID;
            tempFaces.push_back(currNewFace);
        }
    }
    loopSubdiv(tempVerts, tempFaces, nVertices, nFaces, recursion - 1);
}

// This uses the catmull-clark subdivision algorithm to subdivide a mesh.
void ee::catmullClarkSubdiv(const VertBuffer& oVertices, const MeshFaceBuffer& oFaces, VertBuffer& nVertices_res, MeshFaceBuffer& nFaces_res, unsigned rec)
{
    //// keep all of the old vertices:
    //if (rec <= 0)
    //{
    //    nVertices_res = oVertices;
    //    nFaces_res = oFaces;
    //    return;
    //}

    //VertBuffer nVertices = oVertices;
    //MeshFaceBuffer nFaces;

    //struct FacePEdgeP
    //{
    //    GLuint m_facePoint;
    //    
    //    struct EdgePP
    //    {
    //        int m_edgePoint; // the edge point
    //        Edge m_edge; // the edge it is associated with

    //        EdgePP() : m_edgePoint(-1) {}
    //    };
    //    std::array<EdgePP, 3> m_edgePoints; // always associated with 3 because there are three edges per face (it's a triangle)

    //    FacePEdgeP() : m_facePoint(-1) {}
    //};
    //std::vector<FacePEdgeP> facePointPool;
    //facePointPool.resize(oFaces.size()); // the number of faces that we will use

    //std::unordered_map<Edge, std::vector<int>, edgePairHash> edgePointsTable; // edge to faces

    //std::vector<std::vector<int>> vertexFaces;
    //vertexFaces.resize(nVertices.size());
   
    //// assign all of the facePoints:
    //for (int currFaceID = 0; currFaceID < oFaces.size(); currFaceID++)
    //{
    //    const MeshFace& currFace = oFaces[currFaceID]; // the current face

    //    // calculate the facePoint:
    //    glm::vec3 facePoint = (oVertices[currFace.m_indices[0]].m_position +
    //        oVertices[currFace.m_indices[1]].m_position +
    //        oVertices[currFace.m_indices[2]].m_position) * (1.f / 3);
    //    nVertices.push_back(facePoint); // add it to the mster list of vertices
    //    const int currFacePointID = nVertices.size() - 1;

    //    // to be added later:
    //    facePointPool[currFaceID].m_facePoint = currFacePointID;
    //    facePointPool[currFaceID].m_edgePoints[0].m_edge = Edge(currFace.m_indices[0], currFace.m_indices[1]);
    //    facePointPool[currFaceID].m_edgePoints[1].m_edge = Edge(currFace.m_indices[1], currFace.m_indices[2]);
    //    facePointPool[currFaceID].m_edgePoints[2].m_edge = Edge(currFace.m_indices[2], currFace.m_indices[0]);

    //    // associate each of the face's vertices with the current face id
    //    vertexFaces[currFace.m_indices[0]].push_back(currFaceID);
    //    vertexFaces[currFace.m_indices[1]].push_back(currFaceID);
    //    vertexFaces[currFace.m_indices[2]].push_back(currFaceID);

    //    // all the edges
    //    Edge edges[3] = 
    //    {
    //        facePointPool[currFaceID].m_edgePoints[0].m_edge,
    //        facePointPool[currFaceID].m_edgePoints[1].m_edge,
    //        facePointPool[currFaceID].m_edgePoints[2].m_edge
    //    };

    //    // set the faces for each edge (so, each edge gets 2 faces (not worrying about holes right now))
    //    for (int i = 0; i < 3; i++)
    //    {
    //        // check if the edge is already associated:
    //        if (edgePointsTable.find(edges[i]) == edgePointsTable.end())
    //        {
    //            // add the facepoint to the list (by creating a new one first)
    //            edgePointsTable.insert(std::make_pair(edges[i], std::vector<int>({currFaceID})));
    //        }
    //        // if so, add the new face (which is the face itself, not to be mistaken with 
    //        // the point
    //        else
    //        {
    //            edgePointsTable.at(edges[i]).push_back(currFaceID);
    //        }
    //    }
    //}

    //// assign all the edge points
    //for (const auto& edges : edgePointsTable)
    //{
    //    // have to make them pointers so that we actually modify them:
    //    FacePEdgeP* faces[2] = {&facePointPool[edges.second[0]], &facePointPool[edges.second[1]]};
    //    Edge currEdge = edges.first; // the current edge

    //    // calculate the edgepoint for the current edge:
    //    glm::vec3 midEdge = (nVertices[currEdge.p0()].m_position + nVertices[currEdge.p1()].m_position) * 0.5f;
    //    glm::vec3 midFace = (nVertices[faces[0]->m_facePoint].m_position + nVertices[faces[1]->m_facePoint].m_position) * 0.5f;
    //    glm::vec3 edgePoint = (midEdge + midFace) * 0.5f;
    //    nVertices.push_back(edgePoint);
    //    int currEdgePointID = nVertices.size() - 1;

    //    // loop through and update the edgepoint in the appropriate location
    //    // in facePointPool.
    //    for (int face = 0; face < 2; face++)
    //    {
    //        for (int edge = 0; edge < 3; edge++)
    //        {
    //            // if the current edge and the faces edge match, then assign t
    //            if (currEdge == faces[face]->m_edgePoints[edge].m_edge)
    //            {
    //                // now calculate the edge point:
    //                faces[face]->m_edgePoints[edge].m_edgePoint = currEdgePointID;
    //                break;
    //            }
    //        }
    //    }
    //}

    //for (int i = 0; i < facePointPool.size(); i++)
    //{
    //    for (int j = 0; j < 3; j++)
    //        if (facePointPool[i].m_edgePoints[j].m_edgePoint == -1)
    //            assert(false);
    //}

    ////
    //// ARE WE ASSOCIATING THE CORRECT EDGE POINT WITH THE CORRECT POINT (look at how only access two arb. edges at bottom loop)
    ////

    //// go through each point:
    //for (int currPointID = 0; currPointID < vertexFaces.size(); currPointID++)
    //{
    //    const std::vector<int>& faceIDs = vertexFaces[currPointID];
    //    glm::vec3 oldPoint = nVertices[currPointID].m_position;

    //    glm::vec3 aveFacePoint, aveEdgePoint;

    //    // we need a unique set of edge points:
    //    std::set<Edge> edgePoints;
    //    for (int faceID : faceIDs)
    //    {
    //        aveFacePoint += nVertices[facePointPool[faceID].m_facePoint].m_position;
    //        for (int i = 0; i < 3; i++)
    //            if (pointBelongsEdge(currPointID, facePointPool[faceID].m_edgePoints[i].m_edge))
    //                edgePoints.insert(facePointPool[faceID].m_edgePoints[i].m_edge);
    //    }
    //    // now go through the edge points and average them out:
    //    // NOTE: there are NOT edgepoints, but edge's midpoints
    //    // There is a big distinction
    //    for (Edge edgePoint : edgePoints)
    //    {
    //        aveEdgePoint += (nVertices[edgePoint.p0()].m_position + nVertices[edgePoint.p1()].m_position) * 0.5f;
    //    }
    //    aveFacePoint *= (1.f / faceIDs.size());
    //    aveEdgePoint *= (1.f / edgePoints.size());

    //    // the center:
    //    int n = faceIDs.size();
    //    glm::vec3 newCoord = ((n - 3.f) * oldPoint)
    //        + (aveFacePoint)
    //        + (2.f * aveEdgePoint);
    //    newCoord *= 1.f / n;

    //    // update the new vertex as follows:
    //    nVertices[currPointID] = newCoord;

    //    // For each point's face, there can only be two new faces:
    //    for (int faceID : faceIDs)
    //    {
    //        for (int i = 0; i < 2; i++)
    //        {
    //            for (int i = 0; i < 3; i++)
    //            {
    //                if (pointBelongsEdge(currPointID, facePointPool[faceID].m_edgePoints[i].m_edge))
    //                {
    //                    MeshFace newFace;
    //                    newFace.m_indices[0] = currPointID;
    //                    glm::vec3 index0 = nVertices[newFace.m_indices[0]].m_position;
    //                    newFace.m_indices[1] = facePointPool[faceID].m_facePoint;
    //                    glm::vec3 index1 = nVertices[newFace.m_indices[1]].m_position;
    //                    newFace.m_indices[2] = facePointPool[faceID].m_edgePoints[i].m_edgePoint;
    //                    glm::vec3 index2 = nVertices[newFace.m_indices[2]].m_position;
    //                    nFaces.push_back(newFace);
    //                }
    //            }
    //        }
    //    }
    //}

    //nVertices_res = nVertices;
    //nFaces_res = nFaces;

    ////catmullClarkSubdiv(nVertices, nFaces, nVertices_res, nFaces_res, rec - 1);
}
