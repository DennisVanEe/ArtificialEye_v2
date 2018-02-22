#pragma once

#include "SBObject.hpp"
#include "../../Mesh/Mesh.hpp"

namespace ee
{
    class SBVertex : public SBObject
    {
    public:
        SBVertex(Float mass, SBObjectType type, Mesh* model, std::size_t vertexID);

        void update(Float timeStep) override;
        SBObject* getCopy() const override;

    private:
        Mesh* m_model;
        const std::size_t m_vertexID;
    };
}