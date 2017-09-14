#include "SBClosedBodySim.hpp"

ee::SBClosedBodySim::SBClosedBodySim(float P, DynamicMesh* model, float mass, float stiffness, float dampening) :
    SBMeshBasedSim(model, mass, stiffness, dampening),
    m_pressure(addLocalForceGen(&SBPressure(P, model, this)))
{
    addLocalForceGen(&SBPressure(P, model, this));
}

void ee::SBClosedBodySim::setP(float P)
{
    m_pressure->m_P = P;
}

ee::SBClosedBodySim::SBPressure::SBPressure(float P, DynamicMesh* model, SBMeshBasedSim* simulation) :
    m_model(model),
    m_simulation(simulation),
    m_P(P)
{
}

void ee::SBClosedBodySim::SBPressure::applyForces()
{
    // first we get the current volume of the mesh:
    float V = m_model->calcVolume();
    float invV = 1.0 / V;

    // for each face:
    for (std::size_t i = 0; i < m_model->getNumMeshFaces(); i++)
    {
        // get triangle indices:
        MeshFace f = m_model->getMeshFace(i);

        glm::vec3 v0 = m_model->getVertex(f.m_indices[0]).m_position;
        glm::vec3 v1 = m_model->getVertex(f.m_indices[1]).m_position;
        glm::vec3 v2 = m_model->getVertex(f.m_indices[2]).m_position;

        // get current face's area:
        glm::vec3 e0 = v1 - v0;
        glm::vec3 e1 = v2 - v0;
        float A = glm::length(glm::cross(e0, e1)) * 0.5f;

        // get the normal:
        glm::vec3 norm = m_model->getNormal(i);

        glm::vec3 pForce = invV * A * m_P * norm;

        // apply the forces thusly:
        if (m_simulation->getVertexObject(f.m_indices[0])->m_type == SBObjectType::ACTIVE)
        {
            m_simulation->getVertexObject(f.m_indices[0])->m_resultantForce += pForce;
        }
        if (m_simulation->getVertexObject(f.m_indices[1])->m_type == SBObjectType::ACTIVE)
        {
            m_simulation->getVertexObject(f.m_indices[1])->m_resultantForce += pForce;
        }
        if (m_simulation->getVertexObject(f.m_indices[2])->m_type == SBObjectType::ACTIVE)
        {
            m_simulation->getVertexObject(f.m_indices[2])->m_resultantForce += pForce;
        }
    }
}

ee::SBLocalForceGen* ee::SBClosedBodySim::SBPressure::getCopy() const
{
    return new SBPressure(*this);
}