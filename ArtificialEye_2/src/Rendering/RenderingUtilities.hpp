#pragma once

#include <glad/glad.h>
#include "Modeling/Mesh.hpp"

namespace ee
{
    // loads a basic triangle into the situation
    void loadIndexedRectangle(VertBuffer* vertList, IndexBuffer* indexList);
    void loadIndexedCube(VertBuffer* vertList, IndexBuffer* indexList);
    void loadIcosphere(unsigned recursionLevel, VertBuffer* vertList, IndexBuffer* indexList);
}