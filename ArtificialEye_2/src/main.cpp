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
#include "RayTracing/RTMesh.hpp"
#include "RayTracing/RTSphere.hpp"
#include "Rendering/Lens.hpp"
#include "SoftBody/Simulation/SBClosedBodySim.hpp"
#include "SoftBody/ForceGens/SBGravity.hpp"
#include "SoftBody/Constraints/SBPointConstraint.hpp"
#include "SoftBody/Integrators/SBVerletIntegrator.hpp"
#include "SoftBody/Objects/SBFixedPoint.hpp"
#include "SoftBody/SBUtilities.hpp"
#include "Mesh/MeshGenerator.hpp"
#include "Mesh/Subdivision.hpp"

#include <chrono>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace ee;

// These are values you can change:

// Number of  pixels:
const int res_width = ARTIFICIAL_EYE_PROP.photo_res_width;
const int res_height = ARTIFICIAL_EYE_PROP.photo_res_height;

// Total area of retina:
const float maxHeight = ARTIFICIAL_EYE_PROP.distance_height;
const float maxWidth = ARTIFICIAL_EYE_PROP.distance_width;

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

bool g_startSoftBody_hid = false;
bool g_startSoftBody     = true;
bool g_enableWireFram    = false;

struct CSVLine
{
    float tension;
    std::vector<std::uint8_t> pixels;
};

std::vector<CSVLine> g_csvLines;

std::vector<ee::SBPointConstraint*> g_constraints;
std::vector<std::unique_ptr<ee::SBConstraint>>* g_allConstraints;
const float g_constraintMoveSpeed = 0.1f;
float g_totalDisplacement = 0.f;
bool g_defaultP = true;
bool g_switchToEyeView = true;
bool g_addLine = false;
bool g_writeFile = false;
bool g_clearFile = false;
std::ofstream g_csvWriter;

void pushCSVLine(const ImageBuffer* buffer)
{
    CSVLine line;
    line.tension = glm::length2(g_constraints[0]->m_point);

    int size = buffer->getTotalPixelAmt();
    const glm::u8vec3* getData = buffer->getBuffer();
    for (int i = 0; i < size; i++)
    {
        std::uint8_t val = getData[i].r;
        line.pixels.push_back(val);
    }

    g_csvLines.push_back(line);
}

void clearCSVLine()
{
    g_csvLines.clear();
}

void writeCSVLines()
{
    g_csvWriter.open("output.csv", std::ofstream::app);
    if (!g_csvWriter.is_open())
    {
        throw std::exception("Could not create the file output.csv");
    }

    for (const CSVLine& line : g_csvLines)
    {
        g_csvWriter << line.tension;
        for (std::uint8_t b : line.pixels)
        {
            g_csvWriter << "," << static_cast<int>(b);
        }
        g_csvWriter << std::endl;
    }
    g_csvWriter.close();
}

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
    if (action != GLFW_PRESS)
    {
        return;
    }

    float sign;
    switch (key)
    {
    case GLFW_KEY_SPACE:
        g_startSoftBody_hid = !g_startSoftBody_hid;
        break;
    case GLFW_KEY_N:
        g_enableWireFram = !g_enableWireFram;
        break;
    case GLFW_KEY_R:
        g_defaultP = !g_defaultP;
        break;
    case GLFW_KEY_Y:
        g_switchToEyeView = !g_switchToEyeView;
        break;
    case GLFW_KEY_UP:
    case GLFW_KEY_DOWN:
        sign = key == GLFW_KEY_UP ? 1.f : -1.f;

        for (auto& constraint : g_constraints)
        {
            constraint->m_point += g_constraintMoveSpeed * sign * glm::normalize(glm::vec3(constraint->m_point.x, 0.f, constraint->m_point.z));
            g_totalDisplacement += g_constraintMoveSpeed * sign;
        }
    }
}

// TODO: move this to somewhere more appropriate
struct EyePosition
{
    glm::quat rotation;
    glm::vec3 position;
};

std::vector<EyePosition> g_framePositions;

int main()
{
    // THESE ARE CONSTANTS YOU SHOULD CHANGE:
    std::string& FRAME_POSITION_DIR = ARTIFICIAL_EYE_PROP.input_file;
    std::string& OUTPUT_DIR = ARTIFICIAL_EYE_PROP.output_file;

    std::ifstream framePositionStream(FRAME_POSITION_DIR);
    if (!framePositionStream.is_open())
    {
        std::cout << "[ERROR]: Could not open input file: " << FRAME_POSITION_DIR << std::endl;
        return -1;
    }

    std::string line;
    for (int lineIndex = 1; std::getline(framePositionStream, line); lineIndex++)
    {
        if (lineIndex % 2 != 0)
        {
            continue;
        }

        std::istringstream stream(line);

        std::string word;
        EyePosition currFramePos;
        for (int wordIndex = 0; stream >> word; wordIndex++)
        {
            if (wordIndex == 0)
            {
                continue;
            }

            float val = std::atof(word.c_str());
            switch (wordIndex)
            {
            // rotation:
            case 1:
                currFramePos.rotation.x = val;
                break;
            case 2:
                currFramePos.rotation.y = val;
                break;
            case 3:
                currFramePos.rotation.z = val;
                break;
            case 4:
                currFramePos.rotation.w = val;
                break;

            // position:
            case 5:
                currFramePos.position.x = val;
                break;
            case 6:
                currFramePos.position.y = val;
                break;
            case 7:
                currFramePos.position.z = val;
                break;
            default:
                std::cout << "[ERROR]: Issue with frame posiiton syntax." << std::endl;
                return -1;
            }
        }
        currFramePos.position -= glm::vec3(35, - 100, 1848);
        g_framePositions.push_back(currFramePos);
    }

    if (!ARTIFICIAL_EYE_PROP.success)
    {
        return -1;
    }

	RendererParam renderParamTemp;
	renderParamTemp.m_screenWidth  = 480;
	renderParamTemp.m_screenHeight = 480;
	renderParamTemp.m_fov          = 75.f;
	renderParamTemp.m_aspect       = 1400.f / 900.f;
	renderParamTemp.m_near         = 0.1f;
	renderParamTemp.m_far          = 100.f;

    try
    {
        // Initialize the OpenGL renderer for rendering the resulting image:
        CameraParam cameraParams;
        cameraParams.m_position = glm::vec3(10.f, 0.f, 0.f);
        cameraParams.m_up = glm::vec3(0.f, 1.f, 0.f);
        cameraParams.m_yaw = 180.f;
        cameraParams.m_pitch = 0.f;

        // prepare the renderer:
        Renderer::initialize(ARTIFICIAL_EYE_PROP.shader_dir, renderParamTemp, cameraParams);
        Renderer::setCustomKeyboardCallback(setSpaceCallback);
        Renderer::setClearColor(glm::vec3(0));


        // These components here are for rotating the eyeball and positioning the eye.
        glm::mat4 rotationEye = glm::mat4_cast(g_framePositions[0].rotation);
        glm::mat4 positionEye = glm::translate(glm::mat4(1), g_framePositions[0].position);

        // generate the base model transform for placing the object in the world:
        glm::mat4 globalModel = positionEye * rotationEye;

        g_csvWriter.open(OUTPUT_DIR, std::ofstream::out | std::ofstream::trunc);
        g_csvWriter.close();

        // 
        // Scene Sphere
        
		glm::mat4 sceneSphereModel = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 5.f));
        sceneSphereModel = glm::scale(sceneSphereModel, glm::vec3(0.5f));
        Sphere sceneSphere(sceneSphereModel);
		RTSphere rtSceneSphere(&sceneSphere, 0.f, false);

        //
        // Eyeball

        //
        //  Lens & Eyeball

        // The "base transform" for the eyeball and lens. Transforms them so that they are in the proper relative positions
        const glm::mat4 eyeballIntermTransform  = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -2.f)), glm::vec3(2.f * 1.55f));
        const glm::mat4 lensIntermTransform     = glm::scale(glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)), glm::vec3(1.f, ARTIFICIAL_EYE_PROP.lens_thickness, 1.f));
        const glm::mat4 pupilInterimTransform   = glm::mat4(1.f);

        // Update the model transforms for each of these (NOTE: this is done each frame, with globalModel changing each frame)
        glm::mat4 eyeballModel = globalModel * eyeballIntermTransform;
        glm::mat4 lensModel    = globalModel * lensIntermTransform;
        glm::mat4 pupilModel   = globalModel * pupilInterimTransform;

        Sphere eyeball(eyeballModel);
        RTSphere rtEyeball(&eyeball, ARTIFICIAL_EYE_PROP.eyeball_refr_index, false);

        Pupil pupil(pupilModel, 1.f);

        Mesh sphereMesh = loadUVsphere(ARTIFICIAL_EYE_PROP.longitude, ARTIFICIAL_EYE_PROP.latitude);
        Mesh subDivSphereMesh = loopSubdiv(sphereMesh, ARTIFICIAL_EYE_PROP.subdiv_level_lens);
        subDivSphereMesh.calcNormals();
        subDivSphereMesh.setModelTrans(lensModel);

        // Prepare the lens with the appropriate values:
        RTMesh rtLens(&subDivSphereMesh, ARTIFICIAL_EYE_PROP.lens_refr_index, false);
        Lens lensSphere(&sphereMesh, ARTIFICIAL_EYE_PROP.latitude, ARTIFICIAL_EYE_PROP.longitude);

        SBClosedBodySim lensSim(ARTIFICIAL_EYE_PROP.pressure, &sphereMesh, ARTIFICIAL_EYE_PROP.mass, ARTIFICIAL_EYE_PROP.extspring_coeff, ARTIFICIAL_EYE_PROP.extspring_drag);
        lensSim.m_constIterations = ARTIFICIAL_EYE_PROP.iterations;
        lensSim.addIntegrator(&ee::SBVerletIntegrator(1.f / 30.f, ARTIFICIAL_EYE_PROP.extspring_drag));
        addInteriorSpringsUVSphere(&lensSim, ARTIFICIAL_EYE_PROP.latitude, ARTIFICIAL_EYE_PROP.longitude, ARTIFICIAL_EYE_PROP.intspring_coeff, ARTIFICIAL_EYE_PROP.intspring_drag);
        g_constraints = lensSphere.addConstraints(5, &lensSim);
        g_allConstraints = lensSim.getConstraints();

        // The amount to increment the counter by:
		const float incHeight = maxHeight / static_cast<float>(res_height);
		const float incWidth  = maxWidth  / static_cast<float>(res_width);

		std::vector<glm::vec3> pos;
        pos.reserve(res_height * res_width);
		float width, height;
		width = -maxWidth / 2.f;
		for (int w = 0; w < res_width; w++, width += incWidth)
		{
			height = -maxHeight / 2.f;
			for (int h = 0; h < res_height; h++, height += incHeight)
			{
				pos.push_back(glm::vec3(width, height, -1.f));
			}
		}

		// Prepare the image buffer and the ray tracer:
        ImageBuffer imageBuffer(res_width, res_height);
        RayTracer tracer(&pos, &rtLens, &rtEyeball, &rtSceneSphere, &pupil, ARTIFICIAL_EYE_PROP.threads, ARTIFICIAL_EYE_PROP.samples);

        pupil.generateSamples(16);

        // Ray trace the result:
        std::cout << "ray-tracing initial scene..." << std::endl;
        auto before = std::chrono::high_resolution_clock::now();
        tracer.raytraceAll();
        auto after = std::chrono::high_resolution_clock::now();
        std::cout << "stopped-tracing initial scene..." << std::endl;
        std::cout << "time elapsed: " << (after - before).count() << std::endl;

        // Write the paths to an image buffer:
        auto* photoreceptors = tracer.getColors();
		for (int w = 0, uniI = 0; w < res_width; w++)
		{
			for (int h = 0; h < res_height; h++)
			{
				const float color = (*photoreceptors)[uniI];
				imageBuffer.setPixel(w, h, color);
				uniI++;
			}
		}

        int currentFrameIndex = 1;
        bool readyToWrite = true;

        // Render the image buffer if required:
		ee::Renderer::generatePlaneBuffer();
        while (ee::Renderer::isInitialized())
        {
            // update the positions when necessary
            if (currentFrameIndex < g_framePositions.size())
            {
                rotationEye = glm::mat4_cast(g_framePositions[currentFrameIndex].rotation);
                positionEye = glm::translate(glm::mat4(1), g_framePositions[currentFrameIndex].position);

                globalModel = positionEye * rotationEye;

                eyeballModel = globalModel * eyeballIntermTransform;
                lensModel    = globalModel * lensIntermTransform;
                pupilModel   = globalModel * pupilInterimTransform;

                sphereMesh.setModelTrans(lensModel);
                subDivSphereMesh.setModelTrans(lensModel);
                eyeball.setPosition(eyeballModel);
                pupil.pos = pupilModel;

                currentFrameIndex++;
            }
            else
            {
                g_startSoftBody = false;

                if (readyToWrite)
                {
                    writeCSVLines();
                    std::cout << "Wrote result to output." << std::endl;
                    readyToWrite = false;
                }
            }


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

            if (g_startSoftBody || g_startSoftBody_hid)
            {
                lensSim.update(ARTIFICIAL_EYE_PROP.time_step);

                Mesh tempMesh = loopSubdiv(sphereMesh, ARTIFICIAL_EYE_PROP.subdiv_level_lens);
                subDivSphereMesh.updateVertices(tempMesh.vertexBuffer());
                subDivSphereMesh.updateMeshFaces(tempMesh.faceBuffer());
                subDivSphereMesh.calcNormals(); // this might be slow...
                tracer.raytraceAll();

                auto* photoreceptors = tracer.getColors();
                for (int w = 0, uniI = 0; w < res_width; w++)
                {
                    for (int h = 0; h < res_height; h++)
                    {
                        const float color = (*photoreceptors)[uniI];
                        imageBuffer.setPixel(w, h, color);
                        uniI++;
                    }
                }
            }

            pushCSVLine(&imageBuffer);
            std::cout << "Added frame " << currentFrameIndex << " to output." << std::endl;

            Renderer::drawAll();
            Renderer::update(Renderer::timeElapsed());
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

    std::cout << "Exiting the simulation..." << std::endl;
    Renderer::deinitialize();
    return 0;
}