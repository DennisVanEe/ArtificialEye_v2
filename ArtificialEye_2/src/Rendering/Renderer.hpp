//
//  Renderer.hpp
//  Author(s): Dennis van Ee
//

#pragma once

#include <functional>
#include <unordered_map>
#include <memory>
#include <glm/mat4x4.hpp>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Shader.hpp"
#include "Camera.hpp"
#include "TexturePacks/TexturePack.hpp"

namespace ee
{
    struct RendererParam
    {
        int     m_screenWidth;
        int     m_screenHeight;
        float   m_fov;
        float   m_aspect;
        float   m_near;
        float   m_far;
    };

    struct CameraParam
    {
        glm::vec3   m_position;
        glm::vec3   m_up;
        float       m_yaw;
        float       m_pitch;
    };

    using MouseCallbackFunc     =  std::function<void(GLFWwindow*, double, double)>;
    using KeyboardCallbackFunc  =  std::function<void(GLFWwindow*, int, int, int, int)>;
    using ScrollCallbackFunc    =  std::function<void(GLFWwindow*, double, double)>;

    class Drawable;

    namespace Renderer
    {
        // Initialize a renderer with the given parameters:
        void initialize(std::string rootShaderDir, const RendererParam& rendererParam, const CameraParam& cameraParam);
        void deinitialize();

        // used with rendering:
        const Camera* getCamera();
        glm::mat4 getPerspective();

        // used to check if a renderer was initialized (and only can be)
        bool isInitialized();

        void setClearColor(glm::vec3 color);
        void clearBuffers();

        void setCustomMouseCallback(MouseCallbackFunc func);
        void setCustomKeyboardCallback(KeyboardCallbackFunc func);
        void setCustomScrollCallback(ScrollCallbackFunc func);

        float timeElapsed();
        void update(float deltaTime);
        void update();

        void pollEvents();
        void swapBuffers();

        void drawAll();

        // Removing one is not really necessary in this case (it'll happpen at the end)
        // And there aren't any particles in this case that would lead to that.
        TexturePack* getTexturePack(const std::string& name);
        Shader* loadShader(const std::string& vertName, const std::string& fragName, const std::string& geomName = "");
    
        // Implementation:
        // Needed for access by the addTexturePack function:
        namespace impl
        {
            void insertTextPackIntoMap(std::string name, ee::TexturePack* pack);
            bool checkTextPackMap(const std::string& name);
        }

        GLuint getTexture(const std::string& dir);
        void addTexture(std::string dir, GLuint texture);

        // The Drawable is not managed by the renderer
        void addDrawable(Drawable* d);
        // The texture pack IS managed by the renderer (memory managed)
        // and it has to be moved into the result
        template<class T>
        T* addTexturePack(std::string name, T&& pack);
    }
}

#include "Renderer.inl"