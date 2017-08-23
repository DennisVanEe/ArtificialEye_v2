#pragma once

#include "SBConstraint.hpp"
#include "../Objects/SBObject.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBPointConstraint : public SBConstraint
    {
    public:
        SBPointConstraint(Vector3 point, SBObject* obj);

        void satisfyConstraint() override;
        SBConstraint* getCopy() const override;

    public:
        Vector3 m_point;

    private:
        SBObject* m_obj;
    };
}