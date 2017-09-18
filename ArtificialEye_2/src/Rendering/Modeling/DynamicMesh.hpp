#pragma once

#include "Mesh.hpp"

namespace ee
{
    // note: vertexID gauranteed to be incremental (going from 0 to numVertices - 1)
    // A dynamic model is one where the positions of the vertex can change,
    // but not where the number of vertices or their connections can change.
    class DynamicMesh : public Mesh
    {
    public:
        DynamicMesh(std::string textPack, VertBuffer vertices, MeshFaceBuffer indices, int priority = 0);
        DynamicMesh(const DynamicMesh& mesh);
        DynamicMesh(DynamicMesh&& mesh);

        void recalcNormals();

        // the dynamic part:
        void setVertex(const Vertex& vertex, std::size_t vertexID);
    };
}