#pragma once

#include <glad/glad.h>
#include "Modeling/Mesh.hpp"

namespace ee
{
    // loads a basic triangle into the situation
    void loadIndexedRectangle(VertBuffer* vertList, MeshFaceBuffer* indexList);
    void loadIndexedCube(VertBuffer* vertList, MeshFaceBuffer* indexList);
    void loadIcosphere(unsigned recursionLevel, VertBuffer* vertList, MeshFaceBuffer* indexList);
    void loadUVsphere(unsigned nLong, unsigned nLat, VertBuffer* vertList, MeshFaceBuffer* indexList);
}