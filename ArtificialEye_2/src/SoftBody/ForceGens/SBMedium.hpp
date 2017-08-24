#pragma once

#include "SBGlobalForceGen.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBMedium : public SBGlobalForceGen
    {
    public:
        SBMedium(Float dragCoef);

        void applyForce(SBObject* object) override;
        SBGlobalForceGen* getCopy() const override;

    public:
        Float m_dragCoef;
    };
}