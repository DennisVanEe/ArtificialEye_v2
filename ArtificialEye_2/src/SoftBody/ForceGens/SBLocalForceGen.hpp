#pragma once

#include "../Objects/SBObject.hpp"

namespace ee
{
    class SBLocalForceGen
    {
    public:
        virtual void applyForces() = 0;
        virtual SBLocalForceGen* getCopy() const = 0;
    };
}