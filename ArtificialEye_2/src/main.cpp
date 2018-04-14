#define GLM_FORCE_INLINE

#include "Initialization.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/TexturePacks/UniColorTextPack.hpp"
#include "Rendering/TexturePacks/LightUniColorTextpack.hpp"
#include "Rendering/SkyBox.hpp"
#include "Rendering/TexturePacks/SkyBoxTextPack.hpp"
#include "Rendering/TexturePacks/RefractTextPack.hpp"
#include "Rendering/TexturePacks/LineUniColorTextPack.hpp"
#include "Rendering/DrawableMeshContainer.hpp"
#include "Rendering/PlaneBuffer.hpp"
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
#include <glm/gtc/matrix_transform.hpp>

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
const float g_constraintMoveSpeed = 0.1f;
ee::RayTracer* g_tracer;
bool g_defaultP = true;
bool g_switchToEyeView = false;

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

	if (key == GLFW_KEY_Y)
	{
		if (action == GLFW_PRESS)
		{
			g_switchToEyeView = !g_switchToEyeView;
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
        Renderer::setClearColor(glm::vec3(0));

        // loaded cubemap twice, not efficient, but not important right now:
        void* res = Renderer::addTexturePack("refractTextPack", RefractTextPack(glm::vec3(0.f), g_textureDir + g_cubeMapDir, g_cubeMapFaces, ARTIFICIAL_EYE_PROP.lens_refr_index)); assert(res);
        res =       Renderer::addTexturePack("skyBoxTextPack", SkyBoxTextPack(g_textureDir + g_cubeMapDir, g_cubeMapFaces)); assert(res);

        // not drawing the eyeball for now
        // LoadableModel eyeballModel("Models/eyeball/eyebal_fbx.fbx", 1);
        // eyeballModel.load();

        Scene scene;

		glm::mat4 sceneSphereMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 5.f));
		sceneSphereMat = glm::scale(sceneSphereMat, glm::vec3(0.5f));
		RTObjectSphere sceneSphere("sphere", sceneSphereMat, 0.f);
		scene.addObject(&sceneSphere);

		UniColorTextPack spherepack(glm::vec4(0.f));
		Renderer::addTexturePack("sphereTextPack", std::move(spherepack));

		Mesh sceneSphereMesh = loadUVsphere(10, 10);
		sceneSphereMesh.setModelTrans(sceneSphereMat);
		DrawableMeshContainer sceenSphereDrawable(&sceneSphereMesh, "sphereTextPack");

		Renderer::addDrawable(&sceenSphereDrawable);

        // this is for the eyeball model I am using
        // glm::mat4 eyeballTransform = glm::translate(glm::mat4(1), glm::vec3(0.f, 0.f, -2.f));
        // eyeballTransform = glm::scale(eyeballTransform, glm::vec3(1.55f, 1.55f, 1.55f));

        //eyeballModel.setTransform(eyeballTransform);

        //
        //  Lens System:
        //

        glm::mat4 eyeballIntersectionTransform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -2.f));
        eyeballIntersectionTransform = glm::scale(eyeballIntersectionTransform, glm::vec3(2 * 1.55f, 2 * 1.55f, 2 * 1.55f));

        Mesh uvSphereMesh = loadUVsphere(ARTIFICIAL_EYE_PROP.longitude, ARTIFICIAL_EYE_PROP.latitude);
        Mesh uvSubDivSphereMesh = loopSubdiv(uvSphereMesh, ARTIFICIAL_EYE_PROP.subdiv_level_lens);
        Lens lensSphere(&uvSubDivSphereMesh, ARTIFICIAL_EYE_PROP.latitude, ARTIFICIAL_EYE_PROP.longitude);
		uvSubDivSphereMesh.calcNormals();

        DrawableMeshContainer lensDrawable(&uvSubDivSphereMesh, "refractTextPack", true);
        Renderer::addDrawable(&lensDrawable);
        RTObjectMesh rtLens("lens", &uvSubDivSphereMesh, ARTIFICIAL_EYE_PROP.lens_refr_index);

        RTObjectSphere rtEyeBall("eyeball", eyeballIntersectionTransform, ARTIFICIAL_EYE_PROP.eyeball_refr_index);

		// const auto& list = rtObjectMesh.getMesh()->faceBuffer();

		// Mesh* m = &uvSubDivSphereMesh;

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
        lensSim.addIntegrator(&ee::SBVerletIntegrator(1.f / 20.f, ARTIFICIAL_EYE_PROP.extspring_drag));

        // test stuff:
		// generate a simple 480 x 640 screen

        const int res_width = 16;
		const int res_height = 16;

        const float maxHeight = 1.0f;
        const float maxWidth = 1.0f;

		float incHeight = maxHeight / static_cast<float>(res_height);
		float incWidth = maxWidth / static_cast<float>(res_width);

        const int numPhotoreceptorsPosition = res_width * res_height;
        glm::vec3* photoreceptorPositions = new glm::vec3[numPhotoreceptorsPosition];
		float width = -maxWidth / 2.f;
        int photoIndex = 0;

        FramesBuffer framesBuffer(numPhotoreceptorsPosition, 16, FRAMES_BUFFER_MODE::FLOAT1);

		for (int w = 0; w < res_width; w++, width += incWidth)
		{
			float height = -maxHeight / 2.f;
			for (int h = 0; h < res_height; h++, height += incHeight)
			{
                photoreceptorPositions[photoIndex] = glm::vec3(width, height, -1.f);
			}
		}

		ImageBuffer imageBuffer(res_width, res_height);

        g_constraints = lensSphere.addConstraints(5, &lensSim);
        g_tracer = &ee::RayTracer::initialize(photoreceptorPositions, numPhotoreceptorsPosition, &rtLens, &rtEyeBall, &scene, 4, 8, 8);

        g_tracer->raytraceAll();

        std::vector<DrawLine> drawpaths;

        ee::Renderer::addTexturePack("lineTextPack", ee::LineUniColorTextPack(Vec3(0.0, 1.0, 0.0)));
        auto& paths = g_tracer->getRayPaths();
		auto& photoreceptors = g_tracer->getPhotoreceptors();
        for (const auto& p : paths)
        {
            drawpaths.push_back(DrawLine("lineTextPack", p));
        }

        for (auto& p : drawpaths)
        {
            ee::Renderer::addDrawable(&p);
        }

		int uniI = 0;
		for (int w = 0; w < res_width; w++)
		{
			for (int h = 0; h < res_height; h++)
			{
				glm::vec3 color = photoreceptors[uniI].color;
				//std::uint8_t val = static_cast<std::uint8_t>(256.f - (256.f * color.x));
				imageBuffer.setPixel(w, h, color.x);
				uniI++;
			}
		}

		ee::Renderer::generatePlaneBuffer();
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

			if (g_switchToEyeView)
			{
				ee::Renderer::setPlaneBufferTexture(imageBuffer.getTextureID());
			}
			else
			{
				ee::Renderer::unsetPlaneBufferTexture();
			}

            ee::Renderer::clearBuffers();

            float time = Renderer::timeElapsed();
            if (g_startSoftBody)
            {
                lensSim.update(time);
                Mesh tempMesh = loopSubdiv(uvSphereMesh, ARTIFICIAL_EYE_PROP.subdiv_level_lens);
                uvSubDivSphereMesh.updateVertices(tempMesh.vertexBuffer());
                uvSubDivSphereMesh.updateMeshFaces(tempMesh.faceBuffer());
                g_tracer->raytraceAll();

				auto& paths = g_tracer->getRayPaths();
				for (int i = 0; i < drawpaths.size() && i < paths.size(); i++)
				{
					drawpaths[i].setLine(paths[i]);
				}

                auto& photoreceptors = g_tracer->getPhotoreceptors();
                int uniI = 0;
                for (int w = 0; w < res_width; w++)
                {
                    for (int h = 0; h < res_height; h++)
                    {
                        glm::vec3 color = photoreceptors[uniI].color;
                        //std::uint8_t val = static_cast<std::uint8_t>(256.f - (256.f * color.x));
                        imageBuffer.setPixel(w, h, color.x);
                        uniI++;
                    }
                }

				/*for (auto& p : drawpaths)
				{
					ee::Renderer::addDrawable(&p);
				}*/
            }

            Renderer::drawAll();
            Renderer::update(time);
            Renderer::swapBuffers();
            Renderer::pollEvents();
        }

        delete[] photoreceptorPositions;
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