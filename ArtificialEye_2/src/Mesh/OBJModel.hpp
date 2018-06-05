#pragma once

#include "OBJLoader.hpp"
#include "Mesh.hpp"
#include "../Rendering/DrawableMeshContainer.hpp"
#include "../Rendering/TexturePacks/UniColorTextPack.hpp"
#include <vector>

namespace ee
{
    class OBJModel
    {
    public:
        OBJModel(std::string dir)
        {
            objl::Loader loader;
            loader.LoadFile(dir);
            
            // now we loop through each mesh and update it:
            for (objl::Mesh m : loader.LoadedMeshes)
            {
                std::vector<MeshFace> faces;
                for (int i = 0; i < m.Indices.size();)
                {
                    MeshFace face(m.Indices[i++], m.Indices[i++], m.Indices[i++]);
                    faces.push_back(face);
                }
                std::vector<glm::vec3> vertices;
                for (const objl::Vertex v : m.Vertices)
                {
                    glm::vec3 nv;
                    nv.x = v.Position.X;
                    nv.y = v.Position.Y;
                    nv.z = v.Position.Z;
                    vertices.push_back(nv);
                }
                Mesh mesh;
                mesh.create(std::move(vertices), std::move(faces));
                glm::vec3 color(m.MeshMaterial.Ka.X, m.MeshMaterial.Ka.Y, m.MeshMaterial.Ka.Z);
                m_meshes.push_back(std::make_pair(mesh, color));
            }

            int counter = 0;
            UniColorTextPack material(glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
            Renderer::addTexturePack("tempMat", std::move(material));
            for (std::pair<Mesh, glm::vec3>& m : m_meshes)
            {
                //Renderer::addTexturePack("tempMat", std::move(mat));
                m_drawableMeshes.push_back(DrawableMeshContainer(&m.first, "tempMat"));
            }

            for (DrawableMeshContainer& cont : m_drawableMeshes)
            {
                Renderer::addDrawable(&cont);
            }
        }

    private:
        std::vector<std::pair<Mesh, glm::vec3>> m_meshes;
        std::vector<DrawableMeshContainer> m_drawableMeshes;
    };
}