#include "DrawableMeshContainer.hpp"

#include <iostream>

ee::DrawableMeshContainer::DrawableMeshContainer(const Mesh* const mesh, const std::string& textPack, const bool dynamic, const bool sendNormals, const bool sendTextCoords, const int priority) :
    Drawable(textPack, priority), 
    m_mesh(mesh),
	m_sendNormals(sendNormals),
	m_sendTextCoords(sendTextCoords),
    m_type(dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW),
	m_VBO_vertex(0),
	m_VBO_normal(0)
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Prepare the vertices:
	const std::vector<glm::vec3>& updatedVertices = m_mesh->vertexBuffer();
	glGenBuffers(1, &m_VBO_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_vertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * updatedVertices.size(), updatedVertices.data(), m_type);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	if (m_sendNormals)
	{
		const std::vector<glm::vec3>& updatedNormals = m_mesh->normalBuffer();
		glGenBuffers(1, &m_VBO_normal);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO_normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * updatedNormals.size(), updatedNormals.data(), m_type);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

    // Prepare the indices:
	const std::vector<MeshFace>& updatedMeshFaces = m_mesh->faceBuffer();
	glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshFace) * updatedMeshFaces.size(), updatedMeshFaces.data(), m_type);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ee::DrawableMeshContainer::draw()
{
    // Only update this if the mesh wasn't update
    if (m_mesh->wasUpdated())
    {
		const std::vector<glm::vec3>& updatedVertices = m_mesh->vertexBuffer();
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO_vertex);
        glBufferSubData(GL_ARRAY_BUFFER, 0, updatedVertices.size() * sizeof(glm::vec3), updatedVertices.data());
		if (m_sendNormals)
		{
			const std::vector<glm::vec3>& updatedNormals = m_mesh->normalBuffer();
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO_normal);
			glBufferSubData(GL_ARRAY_BUFFER, 0, updatedNormals.size() * sizeof(glm::vec3), updatedNormals.data());
		}
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_mesh->resetUpdated();
    }

    glBindVertexArray(m_VAO);

    const Camera* camera = Renderer::getCamera();

    Drawable::m_shader->use();

    m_texturePack->preDraw(Drawable::m_shader, &m_shaderMaterial, camera, &m_textures); // sets whatever values it may want to set

    Mat4 modelTrans = m_mesh->getModelTrans();

    Mat4 lookAt = camera->viewMatrix();
    Mat4 perspective = Renderer::getPerspective();
    Mat4 trans = perspective * lookAt * modelTrans;

    // all shaders need this:
    Drawable::m_shader->assignMat4("u_posTrans", trans);
    Drawable::m_shader->assignMat4("u_model", modelTrans); // in case this is needed

    glDrawElements(GL_TRIANGLES, m_mesh->getNumIndices(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    m_texturePack->postDraw();
}