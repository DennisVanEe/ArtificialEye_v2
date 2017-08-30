#pragma once

#include "SBObject.hpp"
#include "../../Rendering/Modeling/DynamicMesh.hpp"

namespace ee
{
    class SBVertex : public SBObject
    {
    public:
        SBVertex(Float mass, SBObjectType type, DynamicMesh* model, std::size_t vertexID);

        void update(Float timeStep) override;
        SBObject* getCopy() const override;

    private:
        DynamicMesh* m_model;
        const std::size_t m_vertexID;
    };
}