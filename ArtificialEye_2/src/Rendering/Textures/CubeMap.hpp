#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

#include "Texture.hpp"

namespace ee
{
    class CubeMap : public Texture
    {
    public:
        CubeMap(const CubeMap& other) = default;
        CubeMap(CubeMap&& other);

    private:
        CubeMap(GLuint texture);
        friend CubeMap loadCubeMap(std::string, std::vector<std::string>);
    };

    CubeMap loadCubeMap(std::string rootDir, std::vector<std::string> mapFaces);
}