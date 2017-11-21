#include <unordered_map>
#include <memory>
#include <set>
#include <iostream>

#include "Renderer.hpp"
#include "Drawable.hpp"

void dummyMouseCallback     (GLFWwindow*, double, double) {}
void dummyKeyboardCallBack  (GLFWwindow*, int, int, int, int) {}
void dummyScrollCallBack    (GLFWwindow*, double, double) {}

namespace ee
{
    namespace Renderer
    {
        std::string g_rootShaderDir;
        std::unordered_map<std::string, std::unique_ptr<Shader>>        g_shaders;
        std::unordered_map<std::string, std::unique_ptr<TexturePack>>   g_textPacks;
        std::unordered_map<std::string, GLuint>                         g_textures;
        Camera                                                          g_camera        = Camera(glm::vec3(), glm::vec3(), 0.f, 0.f); // not the cleanest thing I have ever done,
                                                                                                                                      // but I need to initialize it to some dummy value for now.
        // Window and renderer properties:
        glm::mat4                                                       g_perspective;
        GLFWwindow*                                                     g_window;
        glm::vec4                                                       g_clearColor;
        bool                                                            g_initialized;

        // The render queue:
        std::multiset<Drawable*, DrawableCompare>                       g_drawables;

        // these are for custom defintions that someone might want to add
        MouseCallbackFunc                                               g_custMouseCallback;
        KeyboardCallbackFunc                                            g_custKeyboardCallback;
        ScrollCallbackFunc                                              g_custScrollCallback;

        void mouseCallback(GLFWwindow* window, double xpos, double ypos)
        {
            static bool firstMouse = true;
            static double lastX = 0.;
            static double lastY = 0.;

            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            double xoffset = xpos - lastX;
            double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

            lastX = xpos;
            lastY = ypos;

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

void ee::Renderer::initialize(const std::string rootShaderDir, const RendererParam& rendererParam, const CameraParam& cameraParam)
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
    glfwWindowHint(GLFW_SAMPLES, 5);

    g_window = glfwCreateWindow(rendererParam.m_screenWidth, rendererParam.m_screenHeight, PROJ_NAME, nullptr, nullptr);

    if (g_window == nullptr)
    {
        throw std::runtime_error("GLFW could not create a window.");
    }

    glfwMakeContextCurrent(g_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("GLAD could not initialize OpenGL.");
    }

    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(g_window, mouseCallback);
    glfwSetKeyCallback(g_window, keyboardCallback);
    glfwSetScrollCallback(g_window, scrollCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    g_initialized = true;
}

void ee::Renderer::deinitialize()
{
    glfwSetWindowShouldClose(g_window, true);
    glfwTerminate();
    g_initialized = false;

    // deallocate any information here
    for (const auto& text : g_textures)
    {
        glDeleteTextures(1, &(text.second));
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

void ee::Renderer::setClearColor(glm::vec3 color)
{
    g_clearColor = glm::vec4(color, 1.f);
}

void ee::Renderer::clearBuffers()
{
    glClearColor(g_clearColor.r, g_clearColor.g, g_clearColor.b, g_clearColor.a); // clear to the default color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ee::Renderer::setCustomMouseCallback(const MouseCallbackFunc func)
{
    g_custMouseCallback = func;
}

void ee::Renderer::setCustomKeyboardCallback(const KeyboardCallbackFunc func)
{
    g_custKeyboardCallback = func;
}

void ee::Renderer::setCustomScrollCallback(const ScrollCallbackFunc func)
{
    g_custScrollCallback = func;
}

float ee::Renderer::timeElapsed()
{
    static float lastFrameTime = 0.f;
    static float deltaTime = 0.f;

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrameTime;
    lastFrameTime = currentFrame;
    return deltaTime;
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

void ee::Renderer::update(const float deltaTime)
{
    if (glfwGetKey(g_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        deinitialize();
    }

    // position:
    if (glfwGetKey(g_window, GLFW_KEY_W) == GLFW_PRESS)
    {
        g_camera.processKBInput(CameraDir::FORWARD, deltaTime);
    }
    if (glfwGetKey(g_window, GLFW_KEY_S) == GLFW_PRESS)
    {
        g_camera.processKBInput(CameraDir::BACKWARD, deltaTime);
    }
    if (glfwGetKey(g_window, GLFW_KEY_A) == GLFW_PRESS)
    {
        g_camera.processKBInput(CameraDir::LEFT, deltaTime);
    }
    if (glfwGetKey(g_window, GLFW_KEY_D) == GLFW_PRESS)
    {
        g_camera.processKBInput(CameraDir::RIGHT, deltaTime);
    }
}

GLuint ee::Renderer::getTexture(const std::string& dir)
{
    auto it = g_textures.find(dir);
    if (it == g_textures.end())
    {
        return 0;
    }

    return it->second;
}

void ee::Renderer::addTexture(std::string dir, GLuint texture)
{
    g_textures.insert(std::make_pair(dir, texture));
}

void ee::Renderer::addDrawable(Drawable* d)
{
    if (!d->willDraw())
    {
        return; // don't add it (there is no point, you see)
    }

    static bool renderFirstSet = false;
    static bool renderLastSet = false;

    const int priority = d->getPriority();
    if (priority == RENDER_FIRST)
    {
        if (renderFirstSet)
        {
            throw std::logic_error("Can't have two drawables set to RENDER_FIRST");
        }
        else
        {
            renderFirstSet = true;
        }
    }
    else if (priority == RENDER_LAST)
    {
        if (renderLastSet)
        {
            throw std::logic_error("Can't have two drawables set to RENDER_FIRST");
        }
        else
        {
            renderLastSet = true;
        }
    }

    g_drawables.insert(d);
}

void ee::Renderer::drawAll() 
{ 
    for (auto& d : g_drawables) 
    { 
        d->draw(); 
    } 
}

ee::TexturePack* ee::Renderer::getTexturePack(const std::string& name)
{
    if (name.empty())
    {
        return nullptr;
    }

    const auto it = g_textPacks.find(name);
    if (it == g_textPacks.end())
    {
        throw std::runtime_error("TexturePack " + name + " not already loaded.");
    }
    return it->second.get();
}

ee::Shader* ee::Renderer::loadShader(const std::string& vertName, const std::string& fragName, const std::string& geomName)
{
    const std::string key = vertName + "%" + fragName + "%" + geomName;
    const auto loc = g_shaders.find(key);

    if (loc == g_shaders.end())
    {
        const std::string vertDir = g_rootShaderDir + "/" + vertName + ".glsl";
        const std::string fragDir = g_rootShaderDir + "/" + fragName + ".glsl";
        const std::string geomDir = geomName.empty() ? "" : g_rootShaderDir + "/" + geomName + ".glsl";
        
        Shader* const shader = new Shader();
        if (!shader->initialize(vertDir, fragDir, geomDir))
        {
            throw std::runtime_error("Execution stopped because shader program could not be setup.");
        }

        g_shaders.insert(std::make_pair(key, std::unique_ptr<Shader>(shader)));
        return shader;
    }

    return loc->second.get();
}

void ee::Renderer::impl::insertTextPackIntoMap(std::string name, ee::TexturePack* pack)
{
    g_textPacks.insert(std::make_pair(name, std::unique_ptr<ee::TexturePack>(pack)));
}

bool ee::Renderer::impl::checkTextPackMap(const std::string& name)
{
    return g_textPacks.find(name) == g_textPacks.end();
}