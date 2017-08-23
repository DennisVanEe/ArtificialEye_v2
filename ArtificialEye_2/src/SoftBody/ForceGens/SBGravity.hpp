#pragma once

#include "SBForceGen.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBGravity : public SBForceGen
    {
    public:
        SBGravity();
        SBGravity(Vector3 acceleration);

        void applyForce(SBObject* io_object) override;
        SBForceGen* getCopy() const override;

    public:
        Vector3 m_acceleration;
    };
}