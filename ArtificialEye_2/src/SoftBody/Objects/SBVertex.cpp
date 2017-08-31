#include "SBVertex.hpp"

ee::SBVertex::SBVertex(const float mass, const SBObjectType type, DynamicMesh* const model, const std::size_t vertexID) :
    SBObject(mass, type),
    m_model(model),
    m_vertexID(vertexID)
{
    m_currPosition = m_model->getVertex(m_vertexID).m_position;
    m_prevPosition = m_currPosition;
}

void ee::SBVertex::update(float timeStep)
{
    m_model->setVertex(m_currPosition, m_vertexID);
}

ee::SBObject * ee::SBVertex::getCopy() const
{
    return new SBVertex(*this);
}
