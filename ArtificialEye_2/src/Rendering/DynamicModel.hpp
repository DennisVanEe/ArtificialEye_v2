#pragma once

#include "Model.hpp"
#include "../Types.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace ee
{
    // note: vertexID gauranteed to be incremental (going from 0 to numVertices - 1)
    // A dynamic model is one where the positions of the vertex can change,
    // but not where the number of vertices or their connections can change.
    class DynamicModel : public Model
    {
    public:
        DynamicModel(std::string textPack, VertBuffer vertices, IndexBuffer indices);
        DynamicModel(const DynamicModel& model);
        DynamicModel(DynamicModel&& model);

        void setVertex(const Vertex& vertex, std::size_t vertexID);

        void recalcNormals();

    private:
        std::vector<Vector3> m_tempNormals;
    };
}