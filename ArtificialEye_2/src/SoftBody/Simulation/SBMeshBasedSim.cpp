#include "SBMeshBasedSim.hpp"

#include "../Constraints/SBLengthConstraint.hpp"

ee::SBMeshBasedSim::SBMeshBasedSim(Mesh* model, Float mass, Float structStiffness, Float structDampening) :
    SBSimulation(),
    m_model(model)
{
    createSimVertices(mass);
    connectSprings(structStiffness, structDampening);
}

void ee::SBMeshBasedSim::update(Float timeStep)
{
    SBSimulation::update(timeStep);

    // do other stuff that I won't worry about myself
}

void ee::SBMeshBasedSim::addCustomLengthConstraint(Float length, std::size_t vertexID0, std::size_t vertexID1)
{
    SBSimulation::addConstraint(new SBLengthConstraint(length, m_objects[vertexID0].get(), m_objects[vertexID1].get()));
}

void ee::SBMeshBasedSim::createSimVertices(Float mass)
{
    Float vertexMass = mass / m_model->getNumVertices();
    for (std::size_t i = 0; i < m_model->getNumVertices(); i++)
    {
        SBSimulation::addObject(&SBVertex(vertexMass, SBObjectType::ACTIVE, m_model, i));
    }
}

void ee::SBMeshBasedSim::connectSprings(Float structStiffness, Float structDampening)
{
    for (std::size_t i = 0; i < m_model->getNumIndices() - 1; i++)
    {
        GLuint index0 = m_model->getVertexID(i);
        GLuint index1 = m_model->getVertexID(i + 1);
        if (index0 == index1)
        {
            continue; // if we have two that are the same, this could lead to some major issues
        }

        Float zValue = (m_objects[index0]->m_currPosition.y + m_objects[index1]->m_currPosition.y) * 0.5f;
        Float mult = 1 - std::abs(zValue);
        
        const Float stiffness = mult * structStiffness; // so this number decreases as the area of the face increases

        SBSimulation::addSpring(stiffness, structDampening, m_objects[index0].get(), m_objects[index1].get());
        Float length = glm::length(m_objects[index0]->m_currPosition - m_objects[index1]->m_currPosition);
        SBSimulation::addConstraint(new SBLengthConstraint(length, m_objects[index0].get(), m_objects[index1].get()));
    }
}