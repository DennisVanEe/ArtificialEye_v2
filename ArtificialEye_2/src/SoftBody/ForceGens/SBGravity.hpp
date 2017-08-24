#pragma once

#include "SBGlobalForceGen.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBGravity : public SBGlobalForceGen
    {
    public:
        SBGravity();
        SBGravity(Vector3 acceleration);

        void applyForce(SBObject* io_object) override;
        SBGlobalForceGen* getCopy() const override;

    public:
        Vector3 m_acceleration;
    };
}