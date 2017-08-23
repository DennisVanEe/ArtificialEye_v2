#include "SBVertex.hpp"

ee::SBVertex::SBVertex(const Float mass, const SBObjectType type, DynamicModel* const model, const std::size_t vertexID) :
    SBObject(mass, type),
    m_model(model),
    m_vertexID(vertexID)
{
    m_currPosition = m_model->getVertex(m_vertexID).m_position;
    m_prevPosition = m_currPosition;
}

void ee::SBVertex::update(Float timeStep)
{
    m_model->setVertex(m_currPosition, m_vertexID);
}

ee::SBObject * ee::SBVertex::getCopy() const
{
    return new SBVertex(*this);
}
