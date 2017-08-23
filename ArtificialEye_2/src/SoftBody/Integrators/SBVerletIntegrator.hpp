#pragma once

#include "SBIntegrator.hpp"
#include <glm/common.hpp>

namespace ee
{
    class SBVerletIntegrator : public SBIntegrator
    {
    public:
        SBVerletIntegrator(Float timeStep);
        SBVerletIntegrator(Float timeStep, Float drag);

        Float getDrag() const;
        void setDrag(Float drag);

        void integrate(Vector3 acceleration, SBObject* obj) override;

        SBIntegrator* getCopy() const override;

    private:
        Float m_drag;
    };
}