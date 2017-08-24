#pragma once

#include "../Objects/SBObject.hpp"

namespace ee
{
    class SBGlobalForceGen
    {
    public:
        virtual void applyForce(SBObject* io_object) = 0;
        virtual SBGlobalForceGen* getCopy() const = 0;
    };
}