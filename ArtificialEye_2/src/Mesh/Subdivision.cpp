#include "Subdivision.hpp"
#include "Mesh.hpp"
#include "../Types.hpp"
#include <unordered_map>
#include <array>
#include <set>

const int GLUINT_MAX = std::numeric_limits<int>::max();

class Edge
{
public:
    int p0() const { return m_p0; }
    int p1() const { return m_p1; }

    int m_edgePoint;
    std::array<int, 2> m_faces; // doesn't support holes for now

    void set(int p0, int p1) { sort(); }

    bool operator==(const Edge& e) const { return m_p0 == e.m_p0 && m_p1 == e.m_p1; }
    bool operator<(const Edge& e) const { return hash() < e.hash(); }
       
    std::uint64_t hash() const
    {
        std::uint64_t p0 = static_cast<std::uint64_t>(m_p0);
        std::uint64_t p1 = static_cast<std::uint64_t>(m_p1);
        return p0 | p1 << 32; // simple combination hash
    }

    Edge() : m_p0(-1), m_p1(-1), m_edgePoint(-1), m_faces({-1, -1 }) {}
    Edge(int p0, int p1) : m_p0(p0), m_p1(p1), m_edgePoint(-1), m_faces({-1, -1}) { sort(); }

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
    std::uint64_t operator() (const Edge& p) const
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

int lsdCalcEdgeList(Edge edge, std::unordered_map<Edge, EdgePair, EdgePairHash>& edgeList, const std::vector<ee::Vec3>& oldVerts, std::vector<ee::Vec3>& vertices)
{
    auto it = edgeList.find(edge); assert(it != edgeList.end());
    if (it->second.m_edgePoint >= 0)
    {
        return it->second.m_edgePoint;
    }

    const std::vector<int>& pairs = it->second.m_edgePairs;
    ee::Vec3 edgePoint;
    if (pairs.size() == 2)
    {
        edgePoint = 3.f / 8.f * (oldVerts[edge.p0()] + oldVerts[edge.p1()]) +
            1.f / 8.f * (oldVerts[pairs[0]] + oldVerts[pairs[1]]);
    }
    else
    {
        // if there is a boundary case
        edgePoint = 0.5f * (oldVerts[edge.p0()] + oldVerts[edge.p1()]);
    }

    vertices.push_back(edgePoint);
    it->second.m_edgePoint = vertices.size() - 1;
    return it->second.m_edgePoint;
}

ee::Float lsdBetaConst(int n)
{
    if (n <= 3)
    {
        return 3.f / 16.f;
    }

    const ee::Float inner = 3.f / 8.f + 1.f / 4.f * std::cos(ee::PI2 / n);
    return (1.f / n) * (5.f / 8.f - inner * inner);
}

ee::Mesh ee::loopSubdiv(const Mesh& mesh, int recursion)
{
    if (recursion <= 0) { return mesh; }

    std::unordered_map<Edge, EdgePair, EdgePairHash> edgeList; // stores edges and corresponding "pairs"
    std::vector<std::set<int>> vertexList; // stores neighboring points of points

    const std::vector<Vec3>& oVertices = mesh.vertexBuffer();
    const std::vector<MeshFace>& oFaces = mesh.faceBuffer();

    vertexList.resize(oVertices.size());

    // set up both lists:
	int oFacesSize = static_cast<int>(oFaces.size());
    for (int faceID = 0; faceID < oFacesSize; faceID++)
    {
        // prepare the edge faces:
        MeshFace f = oFaces[faceID];
        lsdAddToEdgeList(f[0], f[1], f[2], edgeList);
        lsdAddToEdgeList(f[1], f[2], f[0], edgeList);
        lsdAddToEdgeList(f[2], f[0], f[1], edgeList);

        // prepare the vertexList:
        vertexList[f[0]].insert(f[1]); vertexList[f[0]].insert(f[2]);
        vertexList[f[1]].insert(f[0]); vertexList[f[1]].insert(f[2]);
        vertexList[f[2]].insert(f[1]); vertexList[f[2]].insert(f[0]);
    }

    // now we calculate the new points and the new faces
    std::vector<Vec3> tempVerts;
    tempVerts.resize(oVertices.size());
    std::vector<MeshFace> tempFaces;

    for (MeshFace f : oFaces)
    {
        // some syntactical sugar:
        Edge edges[3] =
        {
            Edge(f[0], f[1]),
            Edge(f[1], f[2]),
            Edge(f[2], f[0])
        };

        // calculate the edge points for this face:
        int edgePoints[3];
        for (int i = 0; i < 3; i++) { edgePoints[i] = lsdCalcEdgeList(edges[i], edgeList, oVertices, tempVerts); }

        MeshFace middleFace;
        middleFace[0] = edgePoints[0];
        Vec3 p0 = tempVerts[edgePoints[0]];
        middleFace[1] = edgePoints[1];
        Vec3 p1 = tempVerts[edgePoints[0]];
        middleFace[2] = edgePoints[2];
        Vec3 p2 = tempVerts[edgePoints[0]];
        tempFaces.push_back(middleFace); // we can do the middle one right now

        // for each vertex of the point:
        for (int i = 0; i < 3; i++)
        {
            int vertID = f[i];

            // calculate the new vertex:
            if (!vertexList[vertID].empty())
            {
                Vec3 sumPoint, oldPoint = oVertices[vertID];
                for (int p : vertexList[vertID])
                {
                    sumPoint += oVertices[p];
                }

                int n = vertexList[vertID].size();
                const Float beta = lsdBetaConst(n);

                Vec3 newPoint = (1 - beta * n) * oldPoint + beta * sumPoint;
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
            currNewFace[0] = belongingEdgePoints[0];
            currNewFace[1] = belongingEdgePoints[1];
            currNewFace[2] = vertID;
            tempFaces.push_back(currNewFace);
        }
    }
	Mesh resultingMesh;
	resultingMesh.create(std::move(tempVerts), std::move(tempFaces));
    return loopSubdiv(std::move(resultingMesh), recursion - 1);
}