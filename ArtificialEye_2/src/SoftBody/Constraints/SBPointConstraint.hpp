#pragma once

#include "SBConstraint.hpp"
#include "../Objects/SBObject.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBPointConstraint : public SBConstraint
    {
    public:
        SBPointConstraint(Vec3 point, SBObject* obj) :
            m_point(point),
            m_obj(obj)
        {
        }

        void satisfyConstraint() override { m_obj->m_currPosition = m_point; }
        SBConstraint* getCopy() const override { return new SBPointConstraint(*this); }

    public:
        Vec3 m_point;

    private:
        SBObject* m_obj;
    };
}