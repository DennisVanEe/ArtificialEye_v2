#include "SBVertex.hpp"
#include "SBFixedPoint.hpp"

ee::SBVertex::SBVertex(const Float mass, const SBObjectType type, Mesh* const model, const std::size_t vertexID) :
    SBObject(mass, type),
    m_model(model),
    m_vertexID(vertexID)
{
    m_currPosition = m_model->getVertex(m_vertexID).m_position;
    m_prevPosition = m_currPosition;
}

void ee::SBVertex::update(Float timeStep)
{
    if (m_model != nullptr)
    {
        m_model->updateVertex(m_currPosition, m_vertexID);
    }
}

ee::SBObject* ee::SBVertex::getCopy() const
{
    return new SBVertex(*this);
}
