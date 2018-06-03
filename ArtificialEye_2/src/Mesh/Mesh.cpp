#include "Mesh.hpp"

#include <fstream>
#include <iomanip>

void ee::Mesh::applyPointTrans(glm::mat4 mat)
{
    for (int i = 0; i < m_vertices.size(); i++)
    {
		m_vertices[i] = transPoint3(mat, m_vertices[i]);
    }
}

glm::vec3 ee::Mesh::getFaceNormal(int meshFaceID) const
{
    const auto& f = m_faces[meshFaceID];

    glm::vec3 v0 = m_vertices[f[0]];
    glm::vec3 v1 = m_vertices[f[1]];
    glm::vec3 v2 = m_vertices[f[2]];

    glm::vec3 e0 = v1 - v0;
    glm::vec3 e1 = v2 - v0;
    return glm::normalize(glm::cross(e0, e1));
}

float ee::Mesh::calcVolume() const
{
    float total = 0.f;

	for (int i = 0; i < m_faces.size(); i++)
    {
		MeshFace f = m_faces[i];
        Float stv = glm::dot(m_vertices[f[0]],
            glm::cross(m_vertices[f[1]], m_vertices[f[2]])) / 6.f;
        total += stv;
    }

    return std::abs(total);
}

void ee::Mesh::calcNormals()
{
	if (m_normals.size() != m_vertices.size())
	{
		m_normals.resize(m_vertices.size());
	}

	// Zero the vector:
	std::memset(m_normals.data(), 0, m_normals.size() * sizeof(float));

    for (int i = 0; i < m_faces.size(); i++)
    {
		const MeshFace f = m_faces[i];

        const glm::vec3 v0 = m_vertices[f[0]];
        const glm::vec3 v1 = m_vertices[f[1]];
        const glm::vec3 v2 = m_vertices[f[2]];

        const glm::vec3 e0 = v1 - v0;
        const glm::vec3 e1 = v2 - v0;
        const glm::vec3 tempNormal = glm::normalize(alignDir(glm::cross(e0, e1), v0 + v1 + v2)); // calculate the temp normal

        m_normals[f[0]] += tempNormal;
		m_normals[f[1]] += tempNormal;
		m_normals[f[2]] += tempNormal;
    }

    // normalize those results and update the model itself:
    for (int i = 0; i < m_normals.size(); i++)
    {
        m_normals[i] = glm::normalize(m_normals[i]);
    }
}

void ee::writeToOBJFile(std::string dir, const Mesh& mesh)
{
    std::ofstream writer(dir, std::ofstream::out | std::ofstream::trunc);
    // write the vertices:
    const std::vector<glm::vec3>& vertices = mesh.vertexBuffer();
    for (glm::vec3 v : vertices)
    {
        writer << "v " << std::setprecision(4) << v.x << " " << std::setprecision(4) << v.y << " " << std::setprecision(4) << v.z << std::endl;
    }

    // write the triangles:
    const std::vector<MeshFace>& faces = mesh.faceBuffer();
    for (MeshFace f : faces)
    {
        writer << "f " << f[0] << " " << f[1] << " " << f[2] << std::endl;
    }

    writer.flush();
    writer.close();
}
