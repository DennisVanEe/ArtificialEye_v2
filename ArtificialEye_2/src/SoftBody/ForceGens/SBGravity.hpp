#pragma once

#include "SBGlobalForceGen.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBGravity : public SBGlobalForceGen
    {
    public:
        SBGravity();
        SBGravity(glm::vec3 acceleration);

        void applyForce(SBObject* io_object) override;
        SBGlobalForceGen* getCopy() const override;

    public:
        glm::vec3 m_acceleration;
    };
}