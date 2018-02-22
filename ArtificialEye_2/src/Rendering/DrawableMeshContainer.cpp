#include "DrawableMeshContainer.hpp"

#include <iostream>

ee::DrawableMeshContainer::DrawableMeshContainer(const Mesh* const mesh, const std::string& textPack, const bool dynamic, const int priority) :
    Drawable(textPack, priority), 
    m_mesh(mesh),
    m_type(dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW)
{
    const std::vector<glm::vec3>& updatedVertices = m_mesh->vertexBuffer();
    const std::vector<glm::vec3>& updatedNormals = m_mesh->normalBuffer();
    const std::vector<MeshFace>& updatedMeshFaces = m_mesh->faceBuffer();

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO_vertex);
    glGenBuffers(1, &m_VBO_normal);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // Prepare the vertices:
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_vertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * updatedVertices.size(), updatedVertices.data(), m_type);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Prepare the normals:
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * updatedNormals.size(), updatedNormals.data(), m_type);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Prepare the indices:
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
        const std::vector<glm::vec3>& updatedNormals = m_mesh->normalBuffer();

        // No point in updating this because it is likely not to be updated
        // m_cachedMeshFaces.insert(m_cachedMeshFaces.end(), updatedMeshFaces.begin(), updatedMeshFaces.end());

        // update the vertices
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO_vertex);
        glBufferSubData(GL_ARRAY_BUFFER, 0, updatedVertices.size() * sizeof(glm::vec3), updatedVertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO_normal);
        glBufferSubData(GL_ARRAY_BUFFER, 0, updatedNormals.size() * sizeof(glm::vec3), updatedNormals.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_cachedMeshFaces.size() * sizeof(MeshFace), m_cachedMeshFaces.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/

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