#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Types.hpp"

namespace ee
{
    class Shader
    {
    public:
        Shader();

        bool initialize(const std::string &vtxPath, const std::string &frgPath, const std::string & geomPath = "");
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(Shader&& shader);

        void use() const;

        void assignTexture(std::string name, int textureUnit);
        void assignMat4(std::string name, const Mat4& mat);
        void assignBool(std::string name, bool value);
        void assignColor(std::string name, Vec3 color);
        void assignColor(std::string name, Vec4 color);
        void assignVec3(std::string name, Vec3 vec);
        void assignfloat(std::string name, Float val);

        void bindTexture(GLenum target, GLuint number, GLuint texture);
        void unbindTexture(GLenum target);

    private:
        GLuint m_programID;
    };
}