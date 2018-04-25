#pragma once

#include "SBConstraint.hpp"
#include "../../Types.hpp"
#include "../Objects/SBObject.hpp"

namespace ee
{
    class SBLengthConstraint : public SBConstraint
    {
    public:
        SBLengthConstraint(Float length, SBObject* objA, SBObject* objB, Float factor = 0.5, int id = 0);

        void satisfyConstraint() override;
        int getID() const
        {
            return m_id;
        }
        SBConstraint* getCopy() const override { return new SBLengthConstraint(*this); }

    public:
        float m_length;
        float m_factor;
        const int m_id;

    private:
        SBObject* m_objA;
        SBObject* m_objB;
    };
}