#pragma once

#include <glad/glad.h>

#include "Modeling/Mesh.hpp"

namespace ee
{
    // loads a basic triangle into the situation
    Mesh loadIndexedRectangle();
    Mesh loadIndexedCube();
    Mesh loadIcosphere(unsigned recursionLevel);
    Mesh loadUVsphere(int nLong, int nLat);
}