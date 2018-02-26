#include "SBClosedBodySim.hpp"

ee::SBClosedBodySim::SBClosedBodySim(Float P, Mesh* model, Float mass, Float stiffness, Float dampening) :
    SBMeshBasedSim(model, mass, stiffness, dampening),
    m_pressure(addLocalForceGen(&SBPressure(P, model, this)))
{
    addLocalForceGen(&SBPressure(P, model, this));
}

void ee::SBClosedBodySim::setP(Float P)
{
    m_pressure->m_P = P;
}

ee::SBClosedBodySim::SBPressure::SBPressure(Float P, Mesh* model, SBMeshBasedSim* simulation) :
    m_model(model),
    m_simulation(simulation),
    m_P(P)
{
}

void ee::SBClosedBodySim::SBPressure::applyForces()
{
    // first we get the current volume of the mesh:
    Float V = m_model->calcVolume();
    Float invV = 1.f / V;

    // for each face:
    for (int i = 0; i < m_model->getNumFaces(); i++)
    {
        // get triangle indices:
        MeshFace f = m_model->getFace(i);

        glm::vec3 v0 = m_model->getVertex(f[0]);
		glm::vec3 v1 = m_model->getVertex(f[1]);
		glm::vec3 v2 = m_model->getVertex(f[2]);

        // get current face's area:
		glm::vec3 e0 = v1 - v0;
		glm::vec3 e1 = v2 - v0;
        float A = glm::length(glm::cross(e0, e1)) * 0.5f;

        // get the normal:
		glm::vec3 norm = m_model->getFaceNormal(i);

		glm::vec3 pForce = invV * A * m_P * norm;

        // apply the forces thusly:
        if (m_simulation->getVertexObject(f[0])->m_type == SBObjectType::ACTIVE)
        {
            m_simulation->getVertexObject(f[0])->m_resultantForce += pForce;
        }
        if (m_simulation->getVertexObject(f[1])->m_type == SBObjectType::ACTIVE)
        {
            m_simulation->getVertexObject(f[1])->m_resultantForce += pForce;
        }
        if (m_simulation->getVertexObject(f[2])->m_type == SBObjectType::ACTIVE)
        {
            m_simulation->getVertexObject(f[2])->m_resultantForce += pForce;
        }
    }
}

ee::SBLocalForceGen* ee::SBClosedBodySim::SBPressure::getCopy() const
{
    return new SBPressure(*this);
}