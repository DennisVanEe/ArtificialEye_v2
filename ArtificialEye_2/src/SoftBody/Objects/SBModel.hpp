#pragma once

#include "SBObject.hpp"
#include "../../Rendering/Model.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBModel : public SBObject
    {
    public:
        SBModel(Model model, Float mass, SBObjectType type);

        void update(Float time) override;

    public:
        Model m_model;
    };
}