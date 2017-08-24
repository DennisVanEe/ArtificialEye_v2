#include "SBClosedBody.hpp"

ee::SBClosedBody::SBClosedBody(Float P, DynamicModel* model, Float mass, Float structStiffness, Float structDampening, Float shearStiffness, Float shearDampening, Float bendStiffness, Float bendDampening) :
    SBClothSim(model, mass, structStiffness, structDampening, shearStiffness, shearDampening, bendStiffness, bendDampening)
{
    SBSimulation::addLocalForceGen(&SBPressure(P,model, this));
}

ee::SBClosedBody::SBPressure::SBPressure(Float P, DynamicModel* model, SBClosedBody* simulation) :
    m_model(model),
    m_simulation(simulation),
    m_P(P)
{
}

void ee::SBClosedBody::SBPressure::applyForces()
{
    // first we get the current volume of the mesh:
    Float V = m_model->calcVolume();
    Float invV = 1 / V;

    // for each face:
    for (std::size_t i = 0; i < m_model->getNumIndices();)
    {
        // get triangle indices:
        std::size_t i0 = m_model->getVertexID(i++);
        std::size_t i1 = m_model->getVertexID(i++);
        std::size_t i2 = m_model->getVertexID(i++);

        Vector3 v0 = m_model->getVertex(i0).m_position;
        Vector3 v1 = m_model->getVertex(i1).m_position;
        Vector3 v2 = m_model->getVertex(i2).m_position;

        // get current face's area:
        Vector3 e0 = v1 - v0;
        Vector3 e1 = v2 - v0;
        Float A = glm::length(glm::cross(e0, e1)) * F(0.5);

        // get the normal:
        Vector3 norm = glm::normalize(glm::cross(e0, e1)); // counter-clockwise winding order

        Vector3 pForce = invV * A * m_P * norm;

        // apply the forces thusly:
        if (m_simulation->m_objects[i0]->m_type == SBObjectType::ACTIVE)
            m_simulation->m_objects[i0]->m_resultantForce += pForce;
        if (m_simulation->m_objects[i1]->m_type == SBObjectType::ACTIVE)
            m_simulation->m_objects[i1]->m_resultantForce += pForce;
        if (m_simulation->m_objects[i2]->m_type == SBObjectType::ACTIVE)
            m_simulation->m_objects[i2]->m_resultantForce += pForce;
    }
}

ee::SBLocalForceGen * ee::SBClosedBody::SBPressure::getCopy() const
{
    return new SBPressure(*this);
}