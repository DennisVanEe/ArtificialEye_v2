#pragma once

#include "SBGlobalForceGen.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBMedium : public SBGlobalForceGen
    {
    public:
        SBMedium(float dragCoef);

        void applyForce(SBObject* object) override;
        SBGlobalForceGen* getCopy() const override;

    public:
        float m_dragCoef;
    };
}