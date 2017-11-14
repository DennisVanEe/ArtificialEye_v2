#pragma once

#include "SBObject.hpp"

namespace ee
{
    class SBFixedPoint : public SBObject
    {
    public:
        SBFixedPoint(Vec3 position) : SBObject(1.0, SBObjectType::PASSIVE) { m_currPosition = position; }
        SBFixedPoint() : SBObject(1.0, SBObjectType::PASSIVE) {}

        void update(Float timeStep) override {}
        SBObject* getCopy() const override { return new SBFixedPoint(*this); }
    };
}