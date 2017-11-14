#include "Lens.hpp"

ee::Lens::Lens(Mesh* mesh, int nLat, int nLon) :
    m_mesh(mesh),
    m_nLatitudes(nLat),
    m_nLongitudes(nLon),
    m_constraintStart(-1),
    m_constraintEnd(-1)
{
    //if (m_mesh->getMeshType() != MeshType::UVSPHERE)
    //{
    //    throw std::logic_error("Lens can only take UVSphere meshes.");
    //}

    m_latitudes.reserve(nLat);
    for (int i = 0; i < nLat; i++)
    {
        std::vector<int> temp;
        int indices = (m_nLongitudes * i) + 1;
        for (int j = 0; j < m_nLongitudes; j++, indices++)
        {
            temp.push_back(indices);
        }
        m_latitudes.push_back(temp);
    }

    m_longitudes.reserve(nLon);
    for (int i = 0; i < nLon; i++)
    {
        std::vector<int> temp;
        for (int j = 0, indices = i + 1; j < nLat; j++, indices += m_nLongitudes)
        {
            temp.push_back(indices);
        }
        m_longitudes.push_back(temp);
    }
}

std::vector<ee::SBPointConstraint*> ee::Lens::addConstraints(int thickness, ee::SBSimulation* sim)
{
    std::vector<ee::SBPointConstraint*> constraints;

    int beginIndex = (m_nLatitudes / 2 - (thickness / 2 + 1)) * (m_nLongitudes) + 1;
    int endIndex;
    for (int i = 0, sub = (thickness / 2 + 1); i < thickness; i++, sub--)
    {
        const int index = (m_nLatitudes / 2 - sub) * (m_nLongitudes) + 1;
        endIndex = index;
        const int end = index + m_nLongitudes;

        for (int j = index; j < end; j++)
        {
            auto ptr = sim->addConstraint(&ee::SBPointConstraint(m_mesh->getVertex(j).m_position, sim->getVertexObject(j)));
            constraints.push_back(ptr);
        }
    }

    m_constraintStart = getLatitudeIndex(beginIndex);
    m_constraintEnd   = getLatitudeIndex(endIndex);

    return std::move(constraints);
}

ee::Mesh* ee::Lens::getMesh()
{
    return m_mesh;
}

const ee::Mesh* ee::Lens::getMesh() const
{
    return m_mesh;
}

int ee::Lens::getNumLatitudes() const
{
    return m_nLatitudes;
}

int ee::Lens::getNumLongitudes() const
{
    return m_nLongitudes;
}

int ee::Lens::getLatitudeIndex(int index) const
{
    return (index - 1) / m_nLongitudes;
}

int ee::Lens::getLongitudeIndex(int index) const
{
    return (index - 1) % m_nLongitudes;
}

glm::vec3 ee::Lens::getNormal(int faceID) const
{
    glm::vec3 norm = m_mesh->getNormal(faceID);
    MeshFace face = m_mesh->getMeshFace(faceID);
    glm::vec3 est = glm::normalize(m_mesh->getVertex(face(0)).m_position + m_mesh->getVertex(face(1)).m_position
        + m_mesh->getVertex(face(2)).m_position);
    return glm::dot(norm, est) > 0 ? norm : -norm;
}

const std::vector<int>& ee::Lens::getLatitudes(int index) const
{
    return m_latitudes[index];
}

const std::vector<int>& ee::Lens::getLongitudes(int index) const
{
    return m_longitudes[index];
}

int ee::Lens::getConstraintStart() const
{
    return m_constraintStart;
}

int ee::Lens::getConstraintEnd() const
{
    return m_constraintEnd;
}
