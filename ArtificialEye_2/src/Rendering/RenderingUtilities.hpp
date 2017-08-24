#pragma once

#include "Model.hpp"

namespace ee
{
    // loads a basic triangle into the situation
    void loadIndexedRectangle(VertBuffer* vertList, IndexBuffer* indexList);
    void loadIndexedCube(VertBuffer* vertList, IndexBuffer* indexList);
}