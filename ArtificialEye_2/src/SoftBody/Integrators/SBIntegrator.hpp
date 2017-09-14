#pragma once

#include "../Objects/SBObject.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBIntegrator
    {
    public:
        SBIntegrator(float constTimeStep);
        float getTimeStep() const;

        virtual void integrate(glm::vec3 acceleration, SBObject* object) = 0;

        virtual SBIntegrator* getCopy() const = 0;

    protected:
        const float m_constTimeStep;
    };
}