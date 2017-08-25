#include <unordered_map>
#include <memory>

#include "Renderer.hpp"

void dummyMouseCallback(GLFWwindow*, double, double) {}
void dummyKeyboardCallBack(GLFWwindow*, int, int, int, int) {}
void dummyScrollCallBack(GLFWwindow*, double, double) {}

namespace ee
{
    namespace Renderer
    {
        std::string g_rootShaderDir;
        std::unordered_map<std::string, std::unique_ptr<Shader>> g_shaders;
        std::unordered_map<std::string, std::unique_ptr<TexturePack>> g_textPacks;
        Camera g_camera = Camera(Vector3(), Vector3(), F(0), F(0)); // not the cleanest thing I have ever done,
                                                                      // but I need to initialize it to some dummy.
        glm::mat4 g_perspective;
        GLFWwindow* g_window;
        bool g_initialized;

        // This information is for time managment

        Float g_lastFrameTime = F(0);
        Float g_deltaTime = F(0);

        Color4 g_clearColor;

        /////////////////////////////
        // CALLBACK FUNCTIONS
        /////////////////////////////

        // these are for custom defintions that someone might want to add
        MouseCallbackFunc g_custMouseCallback;
        KeyboardCallbackFunc g_custKeyboardCallback;
        ScrollCallbackFunc g_custScrollCallback;

        // these are to be used by the callback function
        bool g_firstMouse = true;
        double g_lastX = 0., g_lastY = 0.;

        void mouseCallback(GLFWwindow* window, double xpos, double ypos)
        {
            if (g_firstMouse)
            {
                g_lastX = xpos;
                g_lastY = ypos;
                g_firstMouse = false;
            }

            double xoffset = xpos - g_lastX;
            double yoffset = g_lastY - ypos; // reversed since y-coordinates go from bottom to top

            g_lastX = xpos;
            g_lastY = ypos;

            g_camera.processMInput(xoffset, yoffset);

            g_custMouseCallback(window, xpos, ypos);
        }

        void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            g_custKeyboardCallback(window, key, scancode, action, mods);
        }

        void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
        {
            g_custScrollCallback(window, xoffset, yoffset);
        }
    }
}

const ee::Camera* ee::Renderer::getCamera()
{
    return &g_camera;
}

glm::mat4 ee::Renderer::getPerspective()
{
    return g_perspective;
}

bool ee::Renderer::isInitialized()
{
    return g_initialized;
}

void ee::Renderer::setClearColor(Color3 color)
{
    g_clearColor = Color4(color, 1.f);
}

void ee::Renderer::clearBuffers()
{
    glClearColor(g_clearColor.r, g_clearColor.g, g_clearColor.b, g_clearColor.a); // clear to the default color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ee::Renderer::setCustomMouseCallback(MouseCallbackFunc func)
{
    g_custMouseCallback = func;
}

void ee::Renderer::setCustomKeyboardCallback(KeyboardCallbackFunc func)
{
    g_custKeyboardCallback = func;
}

void ee::Renderer::setCustomScrollCallback(ScrollCallbackFunc func)
{
    g_custScrollCallback = func;
}

ee::Float ee::Renderer::timeElapsed()
{
    Float currentFrame = glfwGetTime();
    g_deltaTime = currentFrame - g_lastFrameTime;
    g_lastFrameTime = currentFrame;
    return g_deltaTime;
}

void ee::Renderer::update()
{
    update(timeElapsed());
}

void ee::Renderer::pollEvents()
{
    glfwPollEvents();
}

void ee::Renderer::swapBuffers()
{
    glfwSwapBuffers(g_window);
}

void ee::Renderer::update(Float deltaTime)
{
    if (glfwGetKey(g_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        deinitialize();
    }

    // position:
    if (glfwGetKey(g_window, GLFW_KEY_W) == GLFW_PRESS)
        g_camera.processKBInput(FORWARD, deltaTime);
    if (glfwGetKey(g_window, GLFW_KEY_S) == GLFW_PRESS)
        g_camera.processKBInput(BACKWARD, deltaTime);
    if (glfwGetKey(g_window, GLFW_KEY_A) == GLFW_PRESS)
        g_camera.processKBInput(LEFT, deltaTime);
    if (glfwGetKey(g_window, GLFW_KEY_D) == GLFW_PRESS)
        g_camera.processKBInput(RIGHT, deltaTime);
}

ee::Renderer::ErrorCode ee::Renderer::initialize(const std::string rootShaderDir, const RendererParam& rendererParam, const CameraParam& cameraParam)
{
    g_custMouseCallback = dummyMouseCallback;
    g_custKeyboardCallback = dummyKeyboardCallBack;
    g_custScrollCallback = dummyScrollCallBack;

    // assign the values:
    g_rootShaderDir = rootShaderDir;
    g_camera = Camera(cameraParam.m_position, cameraParam.m_up, cameraParam.m_yaw, cameraParam.m_pitch);
    g_perspective = glm::perspective(glm::radians(rendererParam.m_fov), rendererParam.m_aspect, rendererParam.m_near, rendererParam.m_far);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_window = glfwCreateWindow(rendererParam.m_screenWidth, rendererParam.m_screenHeight, PROJ_NAME, nullptr, nullptr);

    if (g_window == nullptr)
    {
        return ErrorCode::GLFW_ERROR;
    }

    glfwMakeContextCurrent(g_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return ErrorCode::GLFW_ERROR;
    }

    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(g_window, mouseCallback);
    glfwSetKeyCallback(g_window, keyboardCallback);
    glfwSetScrollCallback(g_window, scrollCallback);

    g_initialized = true;

    // glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    return ErrorCode::SUCCESS;
}

void ee::Renderer::deinitialize()
{
    glfwSetWindowShouldClose(g_window, true);
    glfwTerminate();
    g_initialized = false;
}

void ee::Renderer::insertTextPackIntoMap(std::string name, ee::TexturePack* pack)
{
    std::unique_ptr<ee::TexturePack> smartPtr(pack);
    g_textPacks.insert(std::make_pair(name, std::move(smartPtr)));
}

bool ee::Renderer::checkTextPackMap(std::string name)
{
    return g_textPacks.find(name) == g_textPacks.end();
}

ee::TexturePack* ee::Renderer::getTexturePack(std::string name)
{
    auto it = g_textPacks.find(name);
    if (it == g_textPacks.end())
    {
        return nullptr;
    }
    return it->second.get();
}

ee::Shader* ee::Renderer::loadShader(std::string vertName, std::string fragName)
{
    std::string key = vertName + "%" + fragName;
    auto loc = g_shaders.find(key);

    if (loc == g_shaders.end())
    {
        std::string vertDir = g_rootShaderDir + "/" + vertName + ".glsl";
        std::string fragDir = g_rootShaderDir + "/" + fragName + ".glsl";
        
        std::unique_ptr<Shader> shader(new Shader());
        Shader* result = shader.get();
        if (!shader->initialize(vertDir, fragDir))
        {
            return nullptr;
        }
        g_shaders[key] = std::move(shader);
        return result;
    }
    else
    {
        loc->second.get();
    }
}