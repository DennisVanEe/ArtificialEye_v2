#pragma once

#include "Shader.hpp"
#include "Camera.hpp"
#include <functional>

#include <glm/mat4x4.hpp>
#include <glfw/glfw3.h>

namespace ee
{
    struct RendererParam
    {
        int m_screenWidth, m_screenHeight;
        float m_fov;
        float m_aspect;
        float m_near, m_far;
    };

    struct CameraParam
    {
        glm::vec3 m_position;
        glm::vec3 m_up;
        float m_yaw, m_pitch;
    };

    using MouseCallbackFunc = std::function<void(GLFWwindow*, double, double)>;
    using KeyboardCallbackFunc = std::function<void(GLFWwindow*, int, int, int, int)>;
    using ScrollCallbackFunc = std::function<void(GLFWwindow*, double, double)>;

    namespace Renderer
    {
        enum class ErrorCode {SUCCESS, GLFW_ERROR, GLAD_ERROR };

        ErrorCode initialize(std::string rootShaderDir, const RendererParam& rendererParam, const CameraParam& cameraParam);
        void deinitialize();

        // used with rendering:
        const Camera* getCamera();
        glm::mat4 getPerspective();

        // used to check if a renderer was initialized (and only can be)
        bool isInitialized();

        void setClearColor(Color3 color);
        void clearBuffers();

        void setCustomMouseCallback(MouseCallbackFunc func);
        void setCustomKeyboardCallback(KeyboardCallbackFunc func);
        void setCustomScrollCallback(ScrollCallbackFunc func);

        Float timeElapsed();

        void update(Float deltaTime);
        void update();

        void pollEvents();
        void swapBuffers();

        Shader* loadShader(std::string vertName, std::string fragName);
    };
}