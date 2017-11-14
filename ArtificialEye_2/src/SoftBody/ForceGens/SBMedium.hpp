#pragma once

#include "SBGlobalForceGen.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBMedium : public SBGlobalForceGen
    {
    public:
        SBMedium(Float dragCoef) : m_dragCoef(dragCoef) {}

        void applyForce(SBObject* object) override { object->m_resultantForce -= m_dragCoef * object->m_currVelocity; }
        SBGlobalForceGen* getCopy() const override { return new SBMedium(*this); }

    public:
        Float m_dragCoef;
    };
}