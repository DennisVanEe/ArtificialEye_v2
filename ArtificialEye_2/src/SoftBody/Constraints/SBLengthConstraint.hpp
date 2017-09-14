#pragma once

#include "SBConstraint.hpp"
#include "../../Types.hpp"
#include "../Objects/SBObject.hpp"

namespace ee
{
    class SBLengthConstraint : public SBConstraint
    {
    public:
        SBLengthConstraint(float length, SBObject* objA, SBObject* objB, float factor = 0.5);

        void satisfyConstraint() override;
        SBConstraint* getCopy() const override;

    public:
        float m_length;
        float m_factor;

    private:
        SBObject* m_objA;
        SBObject* m_objB;
    };
}