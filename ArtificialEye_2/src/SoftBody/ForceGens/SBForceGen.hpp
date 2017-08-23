#pragma once

#include "../Objects/SBObject.hpp"

namespace ee
{
    class SBForceGen
    {
    public:
        virtual void applyForce(SBObject* io_object) = 0;
        virtual SBForceGen* getCopy() const = 0;
    };
}