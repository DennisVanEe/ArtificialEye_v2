#include "Sphere.hpp"

ee::Sphere::Sphere() :
    m_position(1.f),
    m_invPosition(1.f)
{
}

ee::Sphere::Sphere(glm::mat4 pos) :
    m_position(pos),
    m_invPosition(glm::inverse(pos))
{
}

glm::mat4 ee::Sphere::getPosition() const
{
    return m_position;
}

glm::mat4 ee::Sphere::getInvPosition() const
{
    return m_invPosition;
}

void ee::Sphere::setPosition(glm::mat4 pos)
{
    m_position = pos;
    m_invPosition = glm::inverse(pos);
}
