#include "SBFixedPoint.hpp"

ee::SBFixedPoint::SBFixedPoint(glm::vec3 position) :
    SBObject(1.f, SBObjectType::PASSIVE)
{
    m_currPosition = position;
}

ee::SBFixedPoint::SBFixedPoint() :
    SBObject(1.f, SBObjectType::PASSIVE)
{
}

void ee::SBFixedPoint::update(float timeStep)
{
}

ee::SBObject * ee::SBFixedPoint::getCopy() const
{
    return new SBFixedPoint(*this);
}