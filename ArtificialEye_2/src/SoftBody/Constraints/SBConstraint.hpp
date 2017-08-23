#pragma once

namespace ee
{
    class SBConstraint
    {
    public:
        virtual void satisfyConstraint() = 0;
        virtual SBConstraint* getCopy() const = 0;
    };
}