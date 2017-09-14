#pragma once

#include "SBConstraint.hpp"
#include "../Objects/SBObject.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBPointConstraint : public SBConstraint
    {
    public:
        SBPointConstraint(glm::vec3 point, SBObject* obj);

        void satisfyConstraint() override;
        SBConstraint* getCopy() const override;

    public:
        glm::vec3 m_point;

    private:
        SBObject* m_obj;
    };
}