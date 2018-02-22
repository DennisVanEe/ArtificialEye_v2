#include "MeshGenerator.hpp"

#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace cube
{
    const ee::VertexBuffer VERTICES =
    {
        // front
        ee::Vec3(-1.0, -1.0,  1.0),
        ee::Vec3( 1.0, -1.0,  1.0),
        ee::Vec3( 1.0,  1.0,  1.0),
        ee::Vec3(-1.0,  1.0,  1.0),
        // back
        ee::Vec3(-1.0, -1.0, -1.0),
        ee::Vec3( 1.0, -1.0, -1.0),
        ee::Vec3( 1.0,  1.0, -1.0),
        ee::Vec3(-1.0,  1.0, -1.0),
    };

    const ee::FaceBuffer INDICES =
    {
        // -- winding is counter-clockwise (facing camera)
        {0, 1, 2},                // pos z
        {0, 2, 3},
        {1, 5, 6},                // pos x
        {1, 6, 2},
        {2, 6, 7},                // pos y
        {2, 7, 3},
        // -- winding is clockwise (facing away from camera)
        {3, 4, 0},                // neg x
        {3, 7, 4},
        {4, 5, 1},                // neg y
        {4, 1, 0},
        {5, 7, 6},                // neg z
        {5, 4, 7},
    };
}

// Based on information from: https://schneide.wordpress.com/2016/07/15/generating-an-icosphere-in-c/
namespace icosphere
{
    const ee::Float t = (1.0 + std::sqrt(5.0)) / 2.0;

    const ee::VertexBuffer VERTICES =
    {
        glm::normalize(ee::Vec3(-1.0,    t,  0.0)),
        glm::normalize(ee::Vec3( 1.0,    t,  0.0)),
        glm::normalize(ee::Vec3(-1.0,   -t,  0.0)),
        glm::normalize(ee::Vec3( 1.0,   -t,  0.0)),

        glm::normalize(ee::Vec3( 0.0, -1.0,    t)),
        glm::normalize(ee::Vec3( 0.0,  1.0,    t)),
        glm::normalize(ee::Vec3( 0.0, -1.0,   -t)),
        glm::normalize(ee::Vec3( 0.0,  1.0,   -t)),

        glm::normalize(ee::Vec3(   t,  0.0, -1.0)),
        glm::normalize(ee::Vec3(   t,  0.0,  1.0)),
        glm::normalize(ee::Vec3(  -t,  0.0, -1.0)),
        glm::normalize(ee::Vec3(  -t,  0.0,  1.0)),
    };

    const ee::FaceBuffer INDICES =
    {
        { 0, 11,  5},
        { 0,  5,  1},
        { 0,  1,  7},
        { 0,  7, 10},
        { 0, 10, 11},

        { 1,  5,  9},
        { 5, 11,  4},
        {11, 10,  2},
        {10,  7,  6},
        { 7,  1,  8},

        { 3,  9,  4},
        { 3,  4,  2},
        { 3,  2,  6},
        { 3,  6,  8},
        { 3,  8,  9},

        { 4,  9,  5},
        { 2,  4, 11},
        { 6,  2, 10},
        { 8,  6,  7},
        { 9,  8,  1},
    };

    ////////////////////
    /// Helper Function:
    ////////////////////

    std::unordered_map<uint64_t, int> g_cachedMiddlePoints;
    int getMiddlePoint(int i0, int i1, ee::VertexBuffer* list)
    {
        uint64_t minInd = static_cast<uint64_t>(std::min(i0, i1));
        uint64_t maxInd = static_cast<uint64_t>(std::max(i0, i1));
        uint64_t key = (minInd << 32) + maxInd;

        auto it = g_cachedMiddlePoints.find(key);
        if (it != g_cachedMiddlePoints.end())
        {
            return it->second;
        }

        ee::Vec3 p0 = (*list)[i0];
        ee::Vec3 p1 = (*list)[i1];
        ee::Vec3 m = glm::normalize((p0 + p1) * 0.5f);

        list->push_back(m);
        g_cachedMiddlePoints.insert(std::make_pair(key, list->size() - 1));
        return list->size() - 1;
    }
}

ee::Mesh ee::loadIndexedRectangle()
{
    VertexBuffer vertList;
    FaceBuffer indexList;

    vertList.push_back(Vec3( 0.5,  0.5, 0.0));
    vertList.push_back(Vec3( 0.5, -0.5, 0.0));
    vertList.push_back(Vec3(-0.5, -0.5, 0.0));
    vertList.push_back(Vec3(-0.5,  0.5, 0.0));

    indexList.push_back({0, 1, 3});
    indexList.push_back({1, 2, 3});

	Mesh mesh;
	mesh.create(std::move(vertList), std::move(indexList));
    return std::move(mesh);
}

ee::Mesh ee::loadIndexedCube()
{
	Mesh mesh;
	mesh.create(cube::VERTICES, cube::INDICES);
	return mesh;
}

ee::Mesh ee::loadIcosphere(int recursionLevel)
{
    icosphere::g_cachedMiddlePoints.clear();
    VertexBuffer vertList = icosphere::VERTICES;
    FaceBuffer indexList = icosphere::INDICES;

    for (int i = 0; i < recursionLevel; i++)
    {
        FaceBuffer tempIndList1;
        for (const MeshFace& face : indexList)
        {
            int i0 = icosphere::getMiddlePoint(face[0], face[1], &vertList);
            int i1 = icosphere::getMiddlePoint(face[1], face[2], &vertList);
            int i2 = icosphere::getMiddlePoint(face[2], face[0], &vertList);

            tempIndList1.push_back({face[0], i0, i2});
            tempIndList1.push_back({face[1], i1, i0});
            tempIndList1.push_back({face[2], i2, i1});
            tempIndList1.push_back({i0, i1, i2});
        }
        indexList = tempIndList1;
    }

	Mesh mesh;
	mesh.create(std::move(vertList), std::move(indexList));
	return mesh;
}

ee::Mesh ee::loadUVsphere(int nLon, int nLat)
{
    if (nLon <= 0 || nLat <= 0)
    {
        throw std::logic_error("The number of longitudinal and latitudinal rings has to be greater than 0");
    }

    VertexBuffer vertList;
    FaceBuffer indexList;

    // Vertices:
    vertList.resize((nLon) * nLat + 2); // plus 1 is for the extra bits in lon side
    vertList[0] = ee::Vec3(0.0, 1.0, 0.0);
    for (unsigned lat = 0; lat < nLat; lat++)
    {
        Float angleLat = glm::pi<Float>() * (Float)(lat + 1) / (nLat + 1);
        Float sinLat = std::sin(angleLat);
        Float cosLat = std::cos(angleLat);

        for (int lon = 0; lon < nLon; lon++)
        {
            Float angleLon = 2 * glm::pi<Float>() * (Float)(lon) / nLon; // (float)(lon == nLon ? 0 : lon) / nLon;
            Float sinLon = std::sin(angleLon);
            Float cosLon = std::cos(angleLon);

            std::size_t index = lon + lat * (nLon) + 1;
            vertList[index] = ee::Vec3(sinLat * cosLon, cosLat, sinLat * sinLon);
        }
    }
    vertList[vertList.size() - 1] = ee::Vec3(0.0, -1.0, 0.0);

    // Indices:
    for (int lon = 0; lon < nLon - 1; lon++)
    {
        indexList.push_back({lon + 2, lon + 1, 0});
    }
    indexList.push_back({1, nLon, 0}); // fix up that seam

    int size = vertList.size();
    for (int lat = 0; lat < nLat - 1; lat++)
    {
        for (int lon = 0; lon < nLon - 1; lon++)
        {
            int current = lon + lat * (nLon) + 1;
            int next = current + nLon;

            indexList.push_back({current, current + 1, next + 1});
            indexList.push_back({current, next + 1, next});
        }
    }

    // fix up seam
    for (int lat = 0; lat < nLat - 1; lat++)
    {
        int current = (nLon - 1) + lat * (nLon)+1;
        int next = current + nLon;

        int current1 = lat * (nLon) + 1;
        int next1 = current1 + nLon;

        indexList.push_back({current, current1, next1});
        indexList.push_back({current, next1, next});
    }

    for (int lon = 0; lon < nLon - 1; lon++)
    {
        indexList.push_back({size - 1, size - (lon + 2) - 1, size - (lon + 1) - 1});
    }
    indexList.push_back({ size - 1, size - 2, size - (nLon)-1 }); // fix up that seam

	Mesh mesh;
	mesh.create(std::move(vertList), std::move(indexList));
	return mesh;
}