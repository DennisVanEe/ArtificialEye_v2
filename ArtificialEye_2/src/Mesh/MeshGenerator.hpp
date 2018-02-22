#pragma once

#include "Mesh.hpp"

namespace ee
{
    // loads a basic triangle into the situation
    Mesh loadIndexedRectangle();
    Mesh loadIndexedCube();
    Mesh loadIcosphere(int recursionLevel);
    Mesh loadUVsphere(int nLong, int nLat);
}