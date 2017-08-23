#include "Rendering/Renderer.hpp"
#include "Rendering/UniColorTextPack.hpp"
#include "Rendering/DynamicModel.hpp"
#include "Rendering/RenderingUtilities.hpp"

#include "SoftBody/Simulation/SBClothSim.hpp"
#include "SoftBody/ForceGens/SBGravity.hpp"
#include "SoftBody/Constraints/SBPointConstraint.hpp"
#include "SoftBody/Integrators/SBVerletIntegrator.hpp"

#include <string>
#include <iostream>

bool setSpace = false;

void setSpaceCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE)
    {
        if (action == GLFW_PRESS)
        {
            setSpace = !setSpace;
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
        ee::loadIndexedRectangle(&vertBuffer, &indBuffer);

        //// now create a model:

        ee::UniColorTextPack textPack(ee::Color4(0.f, 1.f, 0.f, 1.f));

        // load the dynModel
        ee::DynamicModel dynModel(&textPack, std::move(vertBuffer), std::move(indBuffer));

        ee::Renderer::setClearColor(ee::Color3(105 / F(255), 105 / F(255), 105 / F(255)));

        ee::SBClothSim clothSim(&dynModel, F(3), F(2), F(0.02), F(0), F(0), F(0), F(0));
        clothSim.m_constIterations = 10;

        ee::SBGravity* gravity = new ee::SBGravity();
        clothSim.addGlobalForceGen(gravity);

        ee::SBPointConstraint* constraint = new ee::SBPointConstraint(dynModel.getVertex(0).m_position, clothSim.getVertexObject(0));
        clothSim.addConstraint(constraint);

        clothSim.addIntegrator(&ee::SBVerletIntegrator(F(0.01), F(1) / 60));

        while (ee::Renderer::isInitialized())
        {
            ee::Renderer::clearBuffers();

            ee::Float time = ee::Renderer::timeElapsed();
            if (setSpace)
                clothSim.update(time);
            dynModel.draw();

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