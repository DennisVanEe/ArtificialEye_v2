#pragma once

#include "SBObject.hpp"
#include "../../Rendering/Modeling/Mesh.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBModel : public SBObject
    {
    public:
        SBModel(Mesh model, float mass, SBObjectType type);

        void update(float time) override;

    public:
        Mesh m_model;
    };
}