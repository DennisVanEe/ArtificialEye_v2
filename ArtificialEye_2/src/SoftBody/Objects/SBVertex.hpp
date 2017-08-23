#pragma once

#include "SBObject.hpp"
#include "../../Rendering/DynamicModel.hpp"

namespace ee
{
    class SBVertex : public SBObject
    {
    public:
        SBVertex(Float mass, SBObjectType type, DynamicModel* model, std::size_t vertexID);

        void update(Float timeStep) override;
        SBObject* getCopy() const override;

    private:
        DynamicModel* m_model;
        const std::size_t m_vertexID;
    };
}