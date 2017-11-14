#pragma once

#include "SBIntegrator.hpp"
#include <glm/common.hpp>

namespace ee
{
    class SBVerletIntegrator : public SBIntegrator
    {
    public:
        SBVerletIntegrator(Float timeStep) : SBIntegrator(timeStep) {}
        SBVerletIntegrator(Float timeStep, Float drag) :
            SBIntegrator(timeStep),
            m_drag(glm::clamp(drag, 0.0, 1.0)) {}

        Float getDrag() const { return m_drag; }
        void setDrag(Float drag) { m_drag = drag; }

        void integrate(Vec3 acceleration, SBObject* obj) override;

        SBIntegrator* getCopy() const override { return new SBVerletIntegrator(*this); }

    private:
        Float m_drag;
    };
}