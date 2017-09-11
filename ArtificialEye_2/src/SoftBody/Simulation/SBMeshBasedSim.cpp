#include "SBMeshBasedSim.hpp"

#include "../Constraints/SBLengthConstraint.hpp"

ee::SBMeshBasedSim::SBMeshBasedSim(DynamicMesh* model, float mass, float structStiffness, float structDampening) :
    SBSimulation(),
    m_model(model)
{
    createSimVertices(mass);
    connectSprings(structStiffness, structDampening);
}

void ee::SBMeshBasedSim::update(float timeStep)
{
    SBSimulation::update(timeStep);

    // do other stuff that I won't worry about myself
}

void ee::SBMeshBasedSim::addCustomLengthConstraint(float length, std::size_t vertexID0, std::size_t vertexID1)
{
    SBSimulation::addConstraint(new SBLengthConstraint(length, m_objects[vertexID0].get(), m_objects[vertexID1].get()));
}

void ee::SBMeshBasedSim::createSimVertices(float mass)
{
    float vertexMass = mass / m_model->getNumVertices();
    for (std::size_t i = 0; i < m_model->getNumVertices(); i++)
    {
        SBSimulation::addObject(&SBVertex(vertexMass, SBObjectType::ACTIVE, m_model, i));
    }
}

void ee::SBMeshBasedSim::connectSprings(float structStiffness, float structDampening)
{
    for (std::size_t i = 0; i < m_model->getNumIndices() - 1; i++)
    {
        GLuint index0 = m_model->getVertexID(i);
        GLuint index1 = m_model->getVertexID(i + 1);
        if (index0 == index1)
        {
            continue; // if we have two that are the same, this could lead to some major issues
        }

        SBSimulation::addSpring(structStiffness, structDampening, m_objects[index0].get(), m_objects[index1].get());
        float length = glm::length(m_objects[index0]->m_currPosition - m_objects[index1]->m_currPosition);
        SBSimulation::addConstraint(new SBLengthConstraint(length, m_objects[index0].get(), m_objects[index1].get()));
    }
}