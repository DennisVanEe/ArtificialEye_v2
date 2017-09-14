#pragma once

#include "SBObject.hpp"

namespace ee
{
    class SBFixedPoint : public SBObject
    {
    public:
        SBFixedPoint(glm::vec3 position);
        SBFixedPoint();

        void update(float timeStep) override;
        SBObject* getCopy() const override;
    };
}