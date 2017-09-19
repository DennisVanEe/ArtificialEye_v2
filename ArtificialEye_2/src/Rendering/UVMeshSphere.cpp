#include "UVMeshSphere.hpp"

#include "RenderingUtilities.hpp"

ee::UVMeshSphere::UVMeshSphere(Mesh* mesh, unsigned lat, unsigned lon) :
    m_mesh(mesh),
    m_latitudes(lat),
    m_longitudes(lon)
{
}

std::vector<ee::SBPointConstraint*> ee::UVMeshSphere::addConstraints(const std::size_t thickness, ee::SBSimulation * sim)
{
    std::vector<ee::SBPointConstraint*> constraints;

    std::size_t beginIndex = (m_latitudes / 2 - (thickness / 2 + 1)) * (m_longitudes) + 1;
    std::size_t endIndex;
    for (std::size_t i = 0, sub = (thickness / 2 + 1); i < thickness; i++, sub--)
    {
        const std::size_t index = (m_latitudes / 2 - sub) * (m_longitudes) + 1;
        endIndex = index;
        const std::size_t end = index + m_longitudes;

        for (std::size_t j = index; j < end; j++)
        {
            auto ptr = sim->addConstraint(&ee::SBPointConstraint(m_mesh->getVertex(j).m_position, sim->getVertexObject(j)));
            constraints.push_back(ptr);
        }
    }

    m_muscleBegin = getLatIndex(m_latitudes, m_longitudes, beginIndex);
    m_muslceEnd   = getLatIndex(m_latitudes, m_longitudes, endIndex);

    return std::move(constraints);
}

ee::Mesh* ee::UVMeshSphere::getMesh()
{
    return m_mesh;
}

const ee::Mesh* ee::UVMeshSphere::getMesh() const
{
    return m_mesh;
}

unsigned ee::UVMeshSphere::getLatitudes() const
{
    return m_latitudes;
}

unsigned ee::UVMeshSphere::getLongitudes() const
{
    return m_longitudes;
}

unsigned ee::UVMeshSphere::getMuscleEnd() const
{
    return m_muslceEnd;
}

unsigned ee::UVMeshSphere::getMuscleBegin() const
{
    return m_muscleBegin;
}
