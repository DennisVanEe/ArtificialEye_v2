#pragma once

#include "SBConstraint.hpp"
#include "../../Types.hpp"
#include "../Objects/SBObject.hpp"

namespace ee
{
    class SBLengthConstraint : public SBConstraint
    {
    public:
        SBLengthConstraint(float length, SBObject* objA, SBObject* objB);

        void satisfyConstraint() override;
        SBConstraint* getCopy() const override;

    public:
        float m_length;

    private:
        SBObject* m_objA;
        SBObject* m_objB;
    };
}