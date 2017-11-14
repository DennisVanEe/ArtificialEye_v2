#pragma once

#include "EyeballTextPack.hpp"

#include <iostream>

const std::string MATERIAL_NAME = ""; // or "" if nothing
const std::string DIFFUSE_NAME = "textDiffuse";
const std::string SPECULAR_NAME = "textSpec";
const std::string DIFFUSE_CHECK_NAME = "useTD";

void ee::EyeballTextPack::preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures)
{
    if (textures == nullptr)
    {
        throw std::runtime_error("Texture list was null, needed for EyeballTextPack");
    }

    const const std::vector<Texture>& texts = *textures;

    // load the textures here:
    unsigned diffuseCnt = 0;
    unsigned specularCnt = 0;
    for (size_t i = 0; i < texts.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        if (texts[i].m_type == TextType::DIFFUSE)
        {
            shader->assignTexture(MATERIAL_NAME + DIFFUSE_NAME + std::to_string(i), GL_TEXTURE0 + i);
            shader->assignBool(DIFFUSE_CHECK_NAME + std::to_string(i), true);
        }
        else if (texts[i].m_type == TextType::SPECULAR)
        {
            shader->assignTexture(MATERIAL_NAME + SPECULAR_NAME + std::to_string(i), GL_TEXTURE0 + i);
        }
        else assert(false); // this shouldn't happen, so, yeah.

        glBindTexture(GL_TEXTURE_2D, texts[i].getTexture());
    }

    if (texts.size() == 0)
    {
        shader->assignBool("noText", true);
    }
    else
    {
        shader->assignBool("noText", false);
    }
}