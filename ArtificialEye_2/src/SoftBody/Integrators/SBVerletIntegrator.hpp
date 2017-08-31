#pragma once

#include "SBIntegrator.hpp"
#include <glm/common.hpp>

namespace ee
{
    class SBVerletIntegrator : public SBIntegrator
    {
    public:
        SBVerletIntegrator(float timeStep);
        SBVerletIntegrator(float timeStep, float drag);

        float getDrag() const;
        void setDrag(float drag);

        void integrate(Vector3 acceleration, SBObject* obj) override;

        SBIntegrator* getCopy() const override;

    private:
        float m_drag;
    };
}