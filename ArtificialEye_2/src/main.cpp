// #define USE_DOUBLE
#include "Rendering/Renderer.hpp"
#include "Rendering/TexturePacks/UniColorTextPack.hpp"
#include "Rendering/TexturePacks/LightUniColorTextpack.hpp"
#include "Rendering/Modeling/DynamicMesh.hpp"
#include "Rendering/Modeling/SkyBox.hpp"
#include "Rendering/RenderingUtilities.hpp"
#include "Rendering/TexturePacks/SkyBoxTextPack.hpp"
#include "Rendering/TexturePacks/RefractTextPack.hpp"

#include "SoftBody/Simulation/SBClosedBody.hpp"
#include "SoftBody/ForceGens/SBGravity.hpp"
#include "SoftBody/Constraints/SBPointConstraint.hpp"
#include "SoftBody/Integrators/SBVerletIntegrator.hpp"

#include <string>
#include <iostream>
#include <vector>

bool g_startSoftBody = false;
bool g_enableWireFram = false;

ee::SBPointConstraint* g_constraint = nullptr;

const ee::Float DEFAULT_P = F(50);

bool g_defaultP = true;

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

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_UP:
            g_constraint->m_point += ee::Vector3(F(0), F(0.1), F(0));
            break;
        case GLFW_KEY_DOWN:
            g_constraint->m_point += ee::Vector3(F(0), F(-0.1), F(0));
            break;
        case GLFW_KEY_LEFT:
            g_constraint->m_point += ee::Vector3(F(-0.1), F(0), F(0));
            break;
        case GLFW_KEY_RIGHT:
            g_constraint->m_point += ee::Vector3(F(0.1), F(0), F(0));
            break;
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
        rendererParams.m_fov = F(45);
        rendererParams.m_far = F(100);
        rendererParams.m_near = F(0.1);

        cameraParams.m_position = ee::Vector3(10.f, 0.f, 0.f);
        cameraParams.m_up = ee::Vector3(0.f, 1.f, 0.f);
        cameraParams.m_yaw = F(180);
        cameraParams.m_pitch = F(0);

        ee::Renderer::ErrorCode res = ee::Renderer::initialize(sharedDir, rendererParams, cameraParams);
        ee::Renderer::setCustomKeyboardCallback(setSpaceCallback);

        ee::VertBuffer vertBuffer;
        ee::IndexBuffer indBuffer;
        ee::loadIndexedCube(&vertBuffer, &indBuffer);
        // ee::loadIndexedRectangle(&vertBuffer, &indBuffer);

        // now create a model:

        ee::UniColorTextPack uniColor(ee::Color4(0.f, 1.f, 0.f, 1.f));
        ee::Renderer::addTexturePack("uniColor", &uniColor);

        ee::LightUniColorTextPack lightUniColor;
        lightUniColor.m_color = ee::Color3(0.2f, 1.f, 0.f);
        lightUniColor.m_lightPosition = ee::Vector3(2.f, 2.f, 2.f);
        lightUniColor.m_material.m_ambient = ee::Vector3(1.0f, 0.5f, 0.31f);
        lightUniColor.m_material.m_diffuse = ee::Vector3(1.0f, 0.5f, 0.31f);
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

        ee::RefractTextPack refractTextPack(ee::Color3(), "Textures/SkyBox", cubemapcomp, 1.33f);
        ee::SkyBoxTextPack pack("Textures/SkyBox", cubemapcomp);
        ee::Renderer::addTexturePack("SkyBoxTextPack", &pack);
        ee::Renderer::addTexturePack("refractTextPack", &refractTextPack);

        // load the skybox
        ee::SkyBox skyBox("SkyBoxTextPack");
        ee::Renderer::addDrawable(&skyBox);

        // load the dynModel
        ee::DynamicMesh dynModel("refractTextPack", std::move(vertBuffer), std::move(indBuffer));
        dynModel.recalcNormals();
        ee::Renderer::addDrawable(&dynModel);

        ee::Renderer::setClearColor(ee::Color3(105 / F(255), 105 / F(255), 105 / F(255)));

        ee::SBClosedBody clothSim(DEFAULT_P, &dynModel, F(4), F(3), F(2));
        clothSim.m_constIterations = 50;

        ee::SBGravity* gravity = new ee::SBGravity();
        clothSim.addGlobalForceGen(gravity);

        clothSim.addConstraint(&ee::SBPointConstraint(dynModel.getVertex(0).m_position, clothSim.getVertexObject(0)));
        // clothSim.addConstraint(&ee::SBPointConstraint(dynModel.getVertex(1).m_position, clothSim.getVertexObject(1)));
        g_constraint = clothSim.addConstraint(&ee::SBPointConstraint(dynModel.getVertex(6).m_position, clothSim.getVertexObject(6)));
        assert(g_constraint);

        clothSim.addIntegrator(&ee::SBVerletIntegrator(F(1) / 60, F(0.01)));

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
                clothSim.setP(F(0));
            }

            ee::Renderer::clearBuffers();

            ee::Float time = ee::Renderer::timeElapsed();
            if (g_startSoftBody)
            {
                clothSim.update(time);
            }
            
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