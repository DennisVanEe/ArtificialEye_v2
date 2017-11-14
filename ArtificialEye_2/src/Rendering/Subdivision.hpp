#pragma once

#include "Modeling/Mesh.hpp"

namespace ee
{
    Mesh loopSubdiv(const Mesh& mesh, int recursion);
}