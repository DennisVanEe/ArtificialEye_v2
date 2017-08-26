#pragma once

#include "Mesh.hpp"
#include "../../Types.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace ee
{
    // note: vertexID gauranteed to be incremental (going from 0 to numVertices - 1)
    // A dynamic model is one where the positions of the vertex can change,
    // but not where the number of vertices or their connections can change.
    class DynamicMesh : public Mesh
    {
    public:
        DynamicMesh(std::string textPack, VertBuffer vertices, IndexBuffer indices);
        DynamicMesh(const DynamicMesh& model);
        DynamicMesh(DynamicMesh&& model);

        void setVertex(const Vertex& vertex, std::size_t vertexID);
    };
}