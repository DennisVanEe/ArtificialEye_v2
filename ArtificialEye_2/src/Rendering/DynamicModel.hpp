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
        DynamicModel(TexturePack* texturePack, VertBuffer vertices, IndexBuffer indices);
        DynamicModel(const DynamicModel& model);
        DynamicModel(DynamicModel&& model);

        const Vertex& getVertex(std::size_t vertexID) const;
        void setVertex(const Vertex& vertex, std::size_t vertexID);
        std::size_t getNumVertices() const;

        std::size_t getVertexID(std::size_t indexID) const;
        std::size_t getNumIndices() const;
    };
}