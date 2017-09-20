#include "UVMeshSphere.hpp"

#include "RenderingUtilities.hpp"

ee::UVMeshSphere::UVMeshSphere(Mesh* mesh, unsigned nLat, unsigned nLon) :
    m_mesh(mesh),
    m_nLatitudes(nLat),
    m_nLongitudes(nLon),
    m_muscleBegin(nLat),
    m_muslceEnd(nLat)
{
    for (std::size_t i = 0; i < nLat; i++)
    {
        m_latitudes.push_back(getUVSphereLatitude(i, nLon, nLat));
    }

    for (std::size_t i = 0; i < nLon; i++)
    {
        m_longitudes.push_back(getUVSphereLongitude(i, nLon, nLat));
    }
}

std::vector<ee::SBPointConstraint*> ee::UVMeshSphere::addConstraints(const std::size_t thickness, ee::SBSimulation* sim)
{
    std::vector<ee::SBPointConstraint*> constraints;

    std::size_t beginIndex = (m_nLatitudes / 2 - (thickness / 2 + 1)) * (m_nLongitudes) + 1;
    std::size_t endIndex;
    for (std::size_t i = 0, sub = (thickness / 2 + 1); i < thickness; i++, sub--)
    {
        const std::size_t index = (m_nLatitudes / 2 - sub) * (m_nLongitudes) + 1;
        endIndex = index;
        const std::size_t end = index + m_nLongitudes;

        for (std::size_t j = index; j < end; j++)
        {
            auto ptr = sim->addConstraint(&ee::SBPointConstraint(m_mesh->getVertex(j).m_position, sim->getVertexObject(j)));
            constraints.push_back(ptr);
        }
    }

    m_muscleBegin = getLatIndex(m_nLatitudes, m_nLongitudes, beginIndex);
    m_muslceEnd   = getLatIndex(m_nLatitudes, m_nLongitudes, endIndex);

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

unsigned ee::UVMeshSphere::getNumLatitudes() const
{
    return m_nLatitudes;
}

unsigned ee::UVMeshSphere::getNumLongitudes() const
{
    return m_nLongitudes;
}

unsigned ee::UVMeshSphere::getLatitudeIndex(std::size_t index) const
{
    return getLatIndex(m_nLongitudes, m_nLatitudes, index);
}

unsigned ee::UVMeshSphere::getLongitudeIndex(std::size_t index) const
{
    return getLonIndex(m_nLongitudes, m_nLatitudes, index);
}

const std::vector<std::size_t>& ee::UVMeshSphere::getLatitudes(unsigned index) const
{
    return m_latitudes[index];
}

const std::vector<std::size_t>& ee::UVMeshSphere::getLongitudes(unsigned index) const
{
    return m_longitudes[index];
}

unsigned ee::UVMeshSphere::getMuscleEnd() const
{
    return m_muslceEnd;
}

unsigned ee::UVMeshSphere::getMuscleBegin() const
{
    return m_muscleBegin;
}
