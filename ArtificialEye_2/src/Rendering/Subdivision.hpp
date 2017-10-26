#pragma once

#include "Modeling/Mesh.hpp"

namespace ee
{
    void catmullClarkSubdiv(const Mesh* original, Mesh* subdivided);
}