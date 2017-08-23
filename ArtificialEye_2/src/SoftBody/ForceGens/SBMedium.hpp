#pragma once

#include "SBForceGen.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBMedium : public SBForceGen
    {
    public:
        SBMedium(Float dragCoef);

        void applyForce(SBObject* object) override;
        SBForceGen* getCopy() const override;

    public:
        Float m_dragCoef;
    };
}