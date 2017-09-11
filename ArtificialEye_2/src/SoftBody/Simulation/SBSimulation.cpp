#include "SBSimulation.hpp"

void ee::SBSimulation::addSpring(const float stiffness, const float dampening, SBObject* const objA, SBObject* const objB)
{
    m_springs.push_back(std::unique_ptr<SBSpring>(
        new SBSpring(stiffness, dampening, objA, objB)));
}

void ee::SBSimulation::addSpring(const float stiffness, const float dampening, const float length, SBObject* const objA, SBObject* const objB)
{
    m_springs.push_back(std::unique_ptr<SBSpring>(
        new SBSpring(stiffness, dampening, length, objA, objB)));
}

void ee::SBSimulation::addObject(SBObject* const obj)
{
    m_objects.push_back(std::unique_ptr<SBObject>(obj->getCopy()));
}

void ee::SBSimulation::addGlobalForceGen(SBGlobalForceGen* force)
{
    m_globalForceGens.push_back(std::unique_ptr<SBGlobalForceGen>(force->getCopy()));
}

void ee::SBSimulation::addIntegrator(SBIntegrator* integrator)
{
    m_integrator = std::unique_ptr<SBIntegrator>(integrator->getCopy());
}

void ee::SBSimulation::update(float timeStep)
{
    // update the springs:
    for (auto& spring : m_springs)
    {
        spring->applySpringForce();
    }

    // apply the global forces:
    if (m_globalForceGens.size() > 0)
    {
        for (auto& object : m_objects)
        {
            if (object->m_type == SBObjectType::ACTIVE)
            {
                for (auto& force : m_globalForceGens)
                {
                    force->applyForce(object.get());
                }
            }
        }
    }

    // apply yhe local forces:
    for (auto& force : m_localForceGens)
    {
        force->applyForces();
    }

    // integrate:
    Vector3 acceleration;
    for (auto& object : m_objects)
    {
        if (object->m_type == SBObjectType::ACTIVE)
        {
            acceleration = object->m_resultantForce / object->m_mass;
            m_integrator->integrate(acceleration, object.get());
        }
    }

    // apply the constraints:
    for (size_t i = 0; i < m_constIterations; i++)
    {
        for (auto& constraint : m_constraints)
        {
            constraint->satisfyConstraint();
        }
    }

    // update the object's position:
    for (auto& object : m_objects)
    {
        object->update(timeStep);
    }

    // reset them forces:
    for (auto& object : m_objects)
    {
        if (object->m_type == SBObjectType::ACTIVE)
        {
            object->resetForces();
        }
    }
}

ee::SBObject* ee::SBSimulation::getVertexObject(const std::size_t vertexID)
{
    return m_objects[vertexID].get();
}

const ee::SBObject* ee::SBSimulation::getVertexObject(const std::size_t vertexID) const
{
    return getVertexObject(vertexID);
}