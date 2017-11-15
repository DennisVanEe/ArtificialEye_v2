#include "DrawableMeshContainer.hpp"

#include <iostream>

ee::DrawableMeshContainer::DrawableMeshContainer(const Mesh* const mesh, const std::string& textPack, const bool dynamic, const int priority) :
    Drawable(textPack, priority), 
    m_mesh(mesh),
    m_updateCount(0),
    m_type(dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW)
{
    const std::vector<Vertex>& updatedVertices = m_mesh->getVerticesData();
    const std::vector<MeshFace>& updatedMeshFaces = m_mesh->getMeshFaceData();
    castVertexBuffer(updatedVertices);
    m_cachedMeshFaces.insert(m_cachedMeshFaces.end(), updatedMeshFaces.begin(), updatedMeshFaces.end());

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(FloatVertex) * m_cachedVertices.size(), m_cachedVertices.data(), m_type);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshFace) * m_cachedMeshFaces.size(), m_cachedMeshFaces.data(), m_type);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FloatVertex), (void*)offsetof(FloatVertex, m_position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FloatVertex), (void*)offsetof(FloatVertex, m_normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FloatVertex), (void*)offsetof(FloatVertex, m_textCoord));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ee::DrawableMeshContainer::draw()
{
    // Only update this if the mesh wasn't update
    if (m_updateCount != m_mesh->getUpdateCount())
    {
        m_updateCount = m_mesh->getUpdateCount();

        const std::vector<Vertex>& updatedVertices = m_mesh->getVerticesData();
        const std::vector<MeshFace>& updatedMeshFaces = m_mesh->getMeshFaceData();
        castVertexBuffer(updatedVertices);

        // No point in updating this because it is likely not to be updated
        // m_cachedMeshFaces.insert(m_cachedMeshFaces.end(), updatedMeshFaces.begin(), updatedMeshFaces.end());

        // update the vertices
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_cachedVertices.size() * sizeof(FloatVertex), m_cachedVertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_cachedMeshFaces.size() * sizeof(MeshFace), m_cachedMeshFaces.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
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

void ee::DrawableMeshContainer::castVertexBuffer(const std::vector<Vertex>& vertices)
{
    m_cachedVertices.clear();
    for (auto vert : vertices)
    {
        FloatVertex vertex;
        vertex.m_normal = glm::vec3(vert.m_normal);
        vertex.m_position = glm::vec3(vert.m_position);
        vertex.m_textCoord = glm::vec2(vert.m_textCoord);
        m_cachedVertices.push_back(vertex);
    }
}