#pragma once

#include "SBConstraint.hpp"
#include "../../Types.hpp"
#include "../Objects/SBObject.hpp"

namespace ee
{
    class SBLengthConstraint : public SBConstraint
    {
    public:
        SBLengthConstraint(Float length, SBObject* objA, SBObject* objB, Float factor = 0.5);

        void satisfyConstraint() override;
        SBConstraint* getCopy() const override { return new SBLengthConstraint(*this); }

    public:
        float m_length;
        float m_factor;

    private:
        SBObject* m_objA;
        SBObject* m_objB;
    };
}