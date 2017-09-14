#include "Initialization.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/TexturePacks/UniColorTextPack.hpp"
#include "Rendering/TexturePacks/LightUniColorTextpack.hpp"
#include "Rendering/Modeling/DynamicMesh.hpp"
#include "Rendering/Modeling/SkyBox.hpp"
#include "Rendering/Modeling/Line.hpp"
#include "Rendering/RenderingUtilities.hpp"
#include "Rendering/TexturePacks/SkyBoxTextPack.hpp"
#include "Rendering/TexturePacks/RefractTextPack.hpp"
#include "Rendering/TexturePacks/LineUniColorTextPack.hpp"
#include "Rendering/RayTracing/Intersections.hpp"
#include "Rendering/RayTracing/RayTracer.hpp"

#include "SoftBody/Simulation/SBClosedBodySim.hpp"
#include "SoftBody/ForceGens/SBGravity.hpp"
#include "SoftBody/Constraints/SBPointConstraint.hpp"
#include "SoftBody/Integrators/SBVerletIntegrator.hpp"
#include "SoftBody/Objects/SBFixedPoint.hpp"
#include "SoftBody/SBUtilities.hpp"

#include <string>
#include <iostream>
#include <vector>

bool g_startSoftBody = false;
bool g_enableWireFram = false;

std::vector<ee::SBPointConstraint*> g_constraints;
const float g_constraintMoveSpeed = 0.1f;

const float DEFAULT_P = 5.f;

const unsigned g_sphereLat = 49U;
const unsigned g_sphereLon = 49U;

ee::RayTracer* g_tracer;

bool g_defaultP = true;

void addConstraints(const std::size_t thickness, ee::SBSimulation* sim, const ee::Mesh* mesh)
{
    for (std::size_t i = 0, sub = (thickness / 2 + 1); i < thickness; i++, sub--)
    {
        std::size_t index = (g_sphereLat / 2 - sub) * (g_sphereLon)+1;
        std::size_t end = index + g_sphereLon;

        for (std::size_t j = index; j < end; j++)
        {
            auto ptr = sim->addConstraint(&ee::SBPointConstraint(mesh->getVertex(j).m_position, sim->getVertexObject(j)));
            g_constraints.push_back(ptr);
        }
    }
}

void setSpaceCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE)
    {
        if (action == GLFW_PRESS)
        {
            g_startSoftBody = !g_startSoftBody;
        }
    }
    else if (key == GLFW_KEY_N)
    {
        if (action == GLFW_PRESS)
        {
            g_enableWireFram = !g_enableWireFram;
        }
    }
    else if (key == GLFW_KEY_R)
    {
        if (action == GLFW_PRESS)
        {
            g_defaultP = !g_defaultP;
        }
    }

    if (action == GLFW_PRESS && (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN))
    {
        float dir;
        switch (key)
        {
        case GLFW_KEY_UP:
            dir = 1.f;
            break;
        case GLFW_KEY_DOWN:
            dir = -1.f;
            break;
        }

        for (auto& constraint : g_constraints)
        {
            constraint->m_point += g_constraintMoveSpeed * dir * glm::normalize(glm::vec3(constraint->m_point.x, 0.f, constraint->m_point.z));
        }
    }
}

int main()
{
    try
    {
        ee::RendererParam rendererParams;
        ee::CameraParam cameraParams;
        std::string sharedDir = "Shaders";

        rendererParams.m_aspect = 16.f / 9.f;
        rendererParams.m_screenWidth = 1920;
        rendererParams.m_screenHeight = 1080;
        rendererParams.m_fov = 45.f;
        rendererParams.m_far = 100.f;
        rendererParams.m_near = 0.1f;

        cameraParams.m_position = glm::vec3(10.f, 0.f, 0.f);
        cameraParams.m_up = glm::vec3(0.f, 1.f, 0.f);
        cameraParams.m_yaw = 180.f;
        cameraParams.m_pitch = 0.f;

        ee::Renderer::ErrorCode res = ee::Renderer::initialize(sharedDir, rendererParams, cameraParams);
        ee::Renderer::setCustomKeyboardCallback(setSpaceCallback);

        ee::VertBuffer vertBuffer;
        ee::MeshFaceBuffer indBuffer;
        // ee::loadIcosphere(4, &vertBuffer, &indBuffer);
        ee::loadUVsphere(g_sphereLon, g_sphereLat, &vertBuffer, &indBuffer);

        // now create a model:

        ee::UniColorTextPack uniColor(glm::vec4(0.f, 1.f, 0.f, 1.f));
        ee::Renderer::addTexturePack("uniColor", &uniColor);

        ee::LightUniColorTextPack lightUniColor;
        lightUniColor.m_color = glm::vec3(0.2f, 1.f, 0.f);
        lightUniColor.m_lightPosition = glm::vec3(2.f, 2.f, 2.f);
        lightUniColor.m_material.m_ambient = glm::vec3(1.0f, 0.5f, 0.31f);
        lightUniColor.m_material.m_diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
        ee::Renderer::addTexturePack("lightUniColor", &lightUniColor);

        std::vector<std::string> cubemapcomp
        {
            "right.jpg",
            "left.jpg",
            "top.jpg",
            "bottom.jpg",
            "back.jpg",
            "front.jpg"
        };

        ee::RefractTextPack refractTextPack(glm::vec3(), "Textures/SkyBox", cubemapcomp, 1.33f);
        ee::SkyBoxTextPack skyBoxTextPack("Textures/SkyBox", cubemapcomp);
        ee::LineUniColorTextPack lineTextPack(glm::vec3(1.f, 0.f, 0.f));
        ee::Renderer::addTexturePack("SkyBoxTextPack", &skyBoxTextPack);
        ee::Renderer::addTexturePack("refractTextPack", &refractTextPack);
        ee::Renderer::addTexturePack("lineTextPack", &lineTextPack);

        // load the skybox
        ee::SkyBox skyBox("SkyBoxTextPack");
        ee::Renderer::addDrawable(&skyBox);

        // load the dynModel
        ee::DynamicMesh dynModel("refractTextPack", std::move(vertBuffer), std::move(indBuffer));
        ee::Renderer::addDrawable(&dynModel);

        // transform the sphere:
        glm::mat4 modelTrans = glm::rotate(glm::mat4(), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
        modelTrans = glm::scale(modelTrans, glm::vec3(1.f, 0.75f, 1.f));

        // dynModel.applyTransformation(modelTrans);
        dynModel.setModelTrans(modelTrans); // glm::mat4();

        ee::Renderer::setClearColor(glm::vec3(0.45f, 0.45f, 0.45f));

        ///////////////////////////////////////////////////////
        // SIMULATION PARAM
        ///////////////////////////////////////////////////////
        ee::SBClosedBodySim clothSim(DEFAULT_P, &dynModel, 5.f, 3.f, 1.f);
        clothSim.m_constIterations = 5.f; // 10.f;

        ee::addInteriorSpringsUVSphere(&clothSim, g_sphereLat, g_sphereLon, 20.f, 1.f);

        addConstraints(5, &clothSim, &dynModel);

        clothSim.addIntegrator(&ee::SBVerletIntegrator(1.f / 20.f, 0.01f));

        const std::vector<glm::vec3> pos = { glm::vec3(0.f, 0.f, -2.f) };
        ee::RayTracerParam param;
        param.m_widthResolution = 5.f;
        param.m_heightResolution = 5.f;
        g_tracer = &ee::RayTracer::initialize(pos, &dynModel, 1.56f, param, glm::vec3(1.f, 0.f, 0.f));

        while (ee::Renderer::isInitialized())
        {
            if (g_enableWireFram)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            if (g_defaultP)
            {
                clothSim.setP(DEFAULT_P);
            }
            else
            {
                clothSim.setP(0.f);
            }

            ee::Renderer::clearBuffers();

            float time = ee::Renderer::timeElapsed();
            if (g_startSoftBody)
            {
                clothSim.update(time);
            }

            g_tracer->raytrace();

            ee::Renderer::drawAll();
            ee::Renderer::update(time);
            ee::Renderer::swapBuffers();
            ee::Renderer::pollEvents();
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "[EXCEP THROWN]: " << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "Press ENTER to exit." << std::endl;
        std::cin.get();
        return -1;
    }

    return 0;
}