#include "CubeMap.hpp"

#include "../Renderer.hpp"

#include "../../STBI/stb_image.hpp"
#include <sstream>

ee::CubeMap ee::loadCubeMap(std::string rootDir, std::vector<std::string> mapFaces)
{
    const std::string key = rootDir + "?CubeMap";

    GLuint texture = Renderer::getTexture(key);

    if (texture != 0)
    {
        return CubeMap(texture);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    if (mapFaces.size() != 6)
    {
        throw std::runtime_error("Was not provided enough faces for cubemap.");
    }

    int width, height, nChannels;
    for (GLuint i = 0; i < 6; i++)
    {
        unsigned char *data = stbi_load((rootDir + "/" + mapFaces[i]).c_str(), &width, &height, &nChannels, 0);
        if (data)
        {
             glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::stringstream strstrm;
            strstrm << "Could not load cubemap face at path: " << (rootDir + "/" + mapFaces[i]) << std::endl;
            stbi_image_free(data);
            glDeleteTextures(1, &texture);
            throw std::runtime_error(strstrm.str());
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    Renderer::addTexture(key, texture);
    return CubeMap(texture);
}

ee::CubeMap::CubeMap(CubeMap&& other) : 
    Texture(other) 
{ 
}

ee::CubeMap::CubeMap(GLuint texture) : 
    Texture(texture) 
{
}
