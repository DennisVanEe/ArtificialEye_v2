#include "Initialization.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/TexturePacks/UniColorTextPack.hpp"
#include "Rendering/TexturePacks/LightUniColorTextpack.hpp"
#include "Rendering/SkyBox.hpp"
#include "Rendering/TexturePacks/SkyBoxTextPack.hpp"
#include "Rendering/TexturePacks/RefractTextPack.hpp"
#include "Rendering/TexturePacks/LineUniColorTextPack.hpp"
#include "Rendering/DrawableMeshContainer.hpp"
#include "RayTracing/RayTracer.hpp"
#include "RayTracing/Scene.hpp"
#include "RayTracing/EyeBall.hpp"
#include "RayTracing/RTObjectMesh.hpp"
#include "RayTracing/RTObjectSphere.hpp"
#include "Rendering/Lens.hpp"
#include "SoftBody/Simulation/SBClosedBodySim.hpp"
#include "SoftBody/ForceGens/SBGravity.hpp"
#include "SoftBody/Constraints/SBPointConstraint.hpp"
#include "SoftBody/Integrators/SBVerletIntegrator.hpp"
#include "SoftBody/Objects/SBFixedPoint.hpp"
#include "SoftBody/SBUtilities.hpp"
#include "Mesh/MeshGenerator.hpp"
#include "Mesh/Subdivision.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <glm/matrix.hpp>

using namespace ee;

std::vector<std::string> g_cubeMapFaces
{
    "right.jpg",
    "left.jpg",
    "top.jpg",
    "bottom.jpg",
    "back.jpg",
    "front.jpg"
};

const std::string g_textureDir = "Textures/";
const std::string g_cubeMapDir = "SkyBox";

bool g_startSoftBody = false;
bool g_enableWireFram = false;

std::vector<ee::SBPointConstraint*> g_constraints;
const Float g_constraintMoveSpeed = 0.1;
ee::RayTracer* g_tracer;
bool g_defaultP = true;

void addConstraints(const std::size_t thickness, ee::SBSimulation* sim, const ee::Mesh* mesh)
{
    for (std::size_t i = 0, sub = (thickness / 2 + 1); i < thickness; i++, sub--)
    {
        const std::size_t index = (ARTIFICIAL_EYE_PROP.latitude / 2 - sub) * (ARTIFICIAL_EYE_PROP.longitude) + 1;
        const std::size_t end = index + ARTIFICIAL_EYE_PROP.longitude;

        for (std::size_t j = index; j < end; j++)
        {
            auto ptr = sim->addConstraint(&ee::SBPointConstraint(mesh->getVertex(j), sim->getVertexObject(j)));
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
            constraint->m_point += g_constraintMoveSpeed * dir * glm::normalize(Vec3(constraint->m_point.x, 0.0, constraint->m_point.z));
        }
    }
}

int main()
{
    if (!ARTIFICIAL_EYE_PROP.success)
    {
        return -1;
    }

	RendererParam renderParamTemp;
	renderParamTemp.m_screenWidth = 1400;
	renderParamTemp.m_screenHeight = 900;
	renderParamTemp.m_fov = 75.f;
	renderParamTemp.m_aspect = 1400.f / 900.f;
	renderParamTemp.m_near = 0.1f;
	renderParamTemp.m_far = 100.f;

    try
    {
        // The default camera parameters:
        CameraParam cameraParams;
        cameraParams.m_position = glm::vec3(10.f, 0.f, 0.f);
        cameraParams.m_up = glm::vec3(0.f, 1.f, 0.f);
        cameraParams.m_yaw = 180.f;
        cameraParams.m_pitch = 0.f;

        // prepare the renderer:
        Renderer::initialize(ARTIFICIAL_EYE_PROP.shader_dir, renderParamTemp, cameraParams);
        Renderer::setCustomKeyboardCallback(setSpaceCallback);
        Renderer::setClearColor(glm::vec3(0, 0, 0));

        // loaded cubemap twice, not efficient, but not important right now:
        void* res = Renderer::addTexturePack("refractTextPack", RefractTextPack(glm::vec3(0.f, 0.f, 0.f), g_textureDir + g_cubeMapDir, g_cubeMapFaces, ARTIFICIAL_EYE_PROP.lens_refr_index)); assert(res);
        res =       Renderer::addTexturePack("skyBoxTextPack", SkyBoxTextPack(g_textureDir + g_cubeMapDir, g_cubeMapFaces)); assert(res);

        // not drawing the eyeball for now
        // LoadableModel eyeballModel("Models/eyeball/eyebal_fbx.fbx", 1);
        // eyeballModel.load();

        Scene scene;

        // this is for the eyeball model I am using
        glm::mat4 eyeballTransform = glm::translate(glm::mat4(1), glm::vec3(0.0, 0.0, -2.0));
        eyeballTransform = glm::scale(eyeballTransform, glm::vec3(1.55, 1.55, 1.55));

        //eyeballModel.setTransform(eyeballTransform);

        //
        //  Lens System:
        //

        glm::mat4 eyeballIntersectionTransform = glm::translate(glm::mat4(1), glm::vec3(0.0, 0.0, -2.0));
        eyeballIntersectionTransform = glm::scale(eyeballIntersectionTransform, glm::vec3(2 * 1.55, 2 * 1.55, 2 * 1.55));
        Mesh uvSphereMesh = loadUVsphere(ARTIFICIAL_EYE_PROP.longitude, ARTIFICIAL_EYE_PROP.latitude);
        Mesh uvSubDivSphereMesh = loopSubdiv(uvSphereMesh, ARTIFICIAL_EYE_PROP.subdiv_level_lens);
        Lens lensSphere(&uvSubDivSphereMesh, ARTIFICIAL_EYE_PROP.latitude, ARTIFICIAL_EYE_PROP.longitude);
        DrawableMeshContainer lensDrawable(&uvSubDivSphereMesh, "refractTextPack", true);
        Renderer::addDrawable(&lensDrawable);

        EyeBall eyeball(eyeballIntersectionTransform, &uvSubDivSphereMesh);
        RTObjectMesh rtObjectMesh("lens", &uvSubDivSphereMesh, ARTIFICIAL_EYE_PROP.lens_refr_index);
        RTObjectSphere rtObjectSphere("cornea", eyeballIntersectionTransform, ARTIFICIAL_EYE_PROP.eyeball_refr_index);
        scene.addObject(&rtObjectMesh);
        scene.addObject(&rtObjectSphere);

		const auto& list = rtObjectMesh.getMesh()->faceBuffer();

		Mesh* m = &uvSubDivSphereMesh;

        //
        // Superficial Stuff:
        //

        SkyBox skyBox("skyBoxTextPack");
        Renderer::addDrawable(&skyBox);

        // update the positons of the lens
        const glm::mat4 lensModelTrans = glm::scale(glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)), glm::vec3(1.f, ARTIFICIAL_EYE_PROP.lens_thickness, 1.f));
        uvSphereMesh.setModelTrans(lensModelTrans);
        uvSubDivSphereMesh.setModelTrans(lensModelTrans);

        // prepare the simulation
        SBClosedBodySim lensSim(ARTIFICIAL_EYE_PROP.pressure, &uvSphereMesh, ARTIFICIAL_EYE_PROP.mass, ARTIFICIAL_EYE_PROP.extspring_coeff, ARTIFICIAL_EYE_PROP.extspring_drag);
        lensSim.m_constIterations = ARTIFICIAL_EYE_PROP.iterations;
        addInteriorSpringsUVSphere(&lensSim, ARTIFICIAL_EYE_PROP.latitude, ARTIFICIAL_EYE_PROP.longitude, ARTIFICIAL_EYE_PROP.intspring_coeff, ARTIFICIAL_EYE_PROP.intspring_drag);
        //addConstraints(5, &lensSim, &lensMesh);
        lensSim.addIntegrator(&ee::SBVerletIntegrator(1.f / 20.f, ARTIFICIAL_EYE_PROP.extspring_drag));

        // test stuff:
        std::vector<glm::vec3> pos = { glm::vec3(0.f, 0.f, -1.f) };

        g_constraints = lensSphere.addConstraints(5, &lensSim);
        g_tracer = &ee::RayTracer::initialize(pos, &eyeball, &scene, 100, 1, 12);

        uvSubDivSphereMesh.calcNormals();

        //g_tracer->raytraceAll();

        //std::vector<DrawLine> drawpaths;

        //ee::Renderer::addTexturePack("lineTextPack", ee::LineUniColorTextPack(Vec3(0.0, 1.0, 0.0)));
        //auto& paths = g_tracer->getRayPaths();
        //for (auto& p : paths)
        //{
        //    drawpaths.push_back(DrawLine("lineTextPack", p));
        //}

        //for (auto& p : drawpaths)
        //{
        //    ee::Renderer::addDrawable(&p);
        //}

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
                lensSim.setP(ARTIFICIAL_EYE_PROP.pressure);
            }
            else
            {
                lensSim.setP(0.f);
            }

            ee::Renderer::clearBuffers();

            float time = Renderer::timeElapsed();
            if (g_startSoftBody)
            {
                lensSim.update(time);
                Mesh tempMesh = loopSubdiv(uvSphereMesh, ARTIFICIAL_EYE_PROP.subdiv_level_lens);
                // uvSubDivSphereMesh.updateVertices(tempMesh.vertexBuffer()));
                // uvSubDivSphereMesh.updateMeshFaces(tempMesh.vertexBuffer()));
                //g_tracer->raytrace();
            }

            Renderer::drawAll();
            Renderer::update(time);
            Renderer::swapBuffers();
            Renderer::pollEvents();
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "[EXCEP THROWN]: " << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "Press ENTER to exit." << std::endl;
        std::cin.get();
        Renderer::deinitialize();
        return -1;
    }

    Renderer::deinitialize();
    return 0;
}