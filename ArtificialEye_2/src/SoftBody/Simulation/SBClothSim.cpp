#include "SBClothSim.hpp"

#include "../Constraints/SBLengthConstraint.hpp"

ee::SBClothSim::SBClothSim(DynamicModel* model, Float mass, Float structStiffness, Float structDampening, Float shearStiffness, Float shearDampening, Float bendStiffness, Float bendDampening) :
    SBSimulation(),
    m_model(model)
{
    createSimVertices(mass);
    connectSprings(structStiffness, structDampening, shearStiffness, shearDampening, bendStiffness, bendDampening);
}

void ee::SBClothSim::update(Float timeStep)
{
    SBSimulation::update(timeStep);

    // do other stuff that I won't worry about myself
}

ee::SBObject* ee::SBClothSim::getVertexObject(std::size_t vertexID)
{
    return m_objects[vertexID].get();
}

void ee::SBClothSim::createSimVertices(Float mass)
{
    Float vertexMass = mass / m_model->getNumVertices();
    for (std::size_t i = 0; i < m_model->getNumVertices(); i++)
    {
        SBSimulation::addObject(&SBVertex(vertexMass, SBObjectType::ACTIVE, m_model, i));
    }
}

void ee::SBClothSim::connectSprings(Float structStiffness, Float structDampening, Float shearStiffness, Float shearDampening, Float bendStiffness, Float bendDampening)
{
    for (std::size_t i = 0; i < m_model->getNumIndices() - 1; i++)
    {
        GLuint index1 = m_model->getVertexID(i);
        GLuint index2 = m_model->getVertexID(i + 1);
        if (index1 == index2)
        {
            continue; // if we have two that are the same, this could lead to some major issues
        }
        SBSimulation::addSpring(structStiffness, structDampening, m_objects[index1].get(), m_objects[index2].get());

        // this is for now, I guess:
        Float length = glm::length(m_objects[index1]->m_currPosition - m_objects[index2]->m_currPosition);
        SBSimulation::addConstraint(new SBLengthConstraint(length, m_objects[index1].get(), m_objects[index2].get()));
    }
}