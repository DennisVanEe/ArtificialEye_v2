#pragma once

#include "SBGlobalForceGen.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBGravity : public SBGlobalForceGen
    {
    public:
        SBGravity() : m_acceleration(Vec3(0.0, -9.80665, 0.0)) {}
        SBGravity(Vec3 acceleration) : m_acceleration(acceleration) {}

        void applyForce(SBObject* io_object) override { io_object->m_resultantForce += io_object->m_mass * m_acceleration; }
        SBGlobalForceGen* getCopy() const override { return new SBGravity(*this); }

    public:
        const Vec3 m_acceleration;
    };
}