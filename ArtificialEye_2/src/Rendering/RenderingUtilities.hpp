#pragma once

#include <glad/glad.h>
#include "Modeling/Mesh.hpp"

namespace ee
{
    // loads a basic triangle into the situation
    void loadIndexedRectangle(VertBuffer* vertList, MeshFaceBuffer* indexList);
    void loadIndexedCube(VertBuffer* vertList, MeshFaceBuffer* indexList);
    void loadIcosphere(unsigned recursionLevel, VertBuffer* vertList, MeshFaceBuffer* indexList);

    // UV Sphere functions:
    void loadUVsphere(int nLong, int nLat, VertBuffer* vertList, MeshFaceBuffer* indexList);
    std::vector<std::size_t> getUVSphereLatitude(std::size_t index, int nLong, int nLat);
    std::vector<std::size_t> getUVSphereLongitude(std::size_t index, int nLong, int nLat);

    // does not (ever) include the top and bottom points, those points are considered "sporadic"
    int getLatIndex(int nLong, int nLat, std::size_t vecID);
    int getLonIndex(int nLong, int nLat, std::size_t vecID);
}