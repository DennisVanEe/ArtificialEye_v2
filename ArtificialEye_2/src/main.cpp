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

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

// #define INVISIBLE

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
    std::vector<float> pixels;
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

void pushCSVLine(const std::vector<float>& buffer)
{
    CSVLine line;
    line.tension = glm::length2(g_constraints[0]->m_point);
    line.pixels = buffer;
    g_csvLines.push_back(line);
}

void clearCSVLine()
{
    g_csvLines.clear();
}

void writeCSVLines(const std::string& dir)
{
    std::ofstream csvWriter(dir, std::ofstream::out | std::ofstream::trunc);
    if (!csvWriter.is_open())
    {
        throw std::exception("Could not create the file output.csv");
    }

    for (const CSVLine& line : g_csvLines)
    {
        csvWriter << line.tension;
        for (float b : line.pixels)
        {
            csvWriter << "," << std::setprecision(5) << b;
        }
        csvWriter << std::endl;
    }
    csvWriter.close();
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

void captureFrame(int frameIndex, const RayTracer& tracer, ImageBuffer* imageBuffer)
{
    const std::vector<float>& photoreceptors = tracer.getColors();
#ifndef INVISIBLE
    for (int w = 0, uniI = 0; w < res_width; w++)
    {
        for (int h = 0; h < res_height; h++, uniI++)
        {
            imageBuffer->setPixel(w, h, photoreceptors[uniI]);
        }
    }
#endif
    pushCSVLine(photoreceptors);
    std::cout << "Finished frame: " << frameIndex << std::endl;
}

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
                std::cout << "[ERROR]: Issue with frame position syntax." << std::endl;
                return -1;
            }
        }
        currFramePos.position -= glm::vec3(35, -100, 1848);
        g_framePositions.push_back(currFramePos);
    }

    if (!ARTIFICIAL_EYE_PROP.success)
    {
        return -1;
    }

    RendererParam renderParamTemp;
    renderParamTemp.m_screenWidth = 1280;
    renderParamTemp.m_screenHeight = 720;
    renderParamTemp.m_fov = 75.f;
    renderParamTemp.m_aspect = 1400.f / 900.f;
    renderParamTemp.m_near = 0.1f;
    renderParamTemp.m_far = 100.f;

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
        Renderer::setClearColor(glm::vec3(0.f));

        SkyBoxTextPack skyboxMaterial(g_textureDir + g_cubeMapDir, g_cubeMapFaces);
        Renderer::addTexturePack("skyboxMaterial", std::move(skyboxMaterial));
        SkyBox skybox("skyboxMaterial");
        Renderer::addDrawable(&skybox);

        // These components here are for rotating the eyeball and positioning the eye.
        glm::mat4 rotationEye = glm::mat4_cast(g_framePositions[0].rotation);
        glm::mat4 positionEye = glm::translate(glm::mat4(1), g_framePositions[0].position);

        // generate the base model transform for placing the object in the world:
        glm::mat4 globalModel = positionEye * rotationEye;

        // 
        // Scene Sphere

        glm::mat4 sceneSphereModel = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 5.f));
        sceneSphereModel = glm::scale(sceneSphereModel, glm::vec3(0.5f));
        Sphere sceneSphere(sceneSphereModel);
        RTSphere rtSceneSphere(&sceneSphere, 0.f, false);

        // Add the drawable element of the scene sphere:
        UniColorTextPack sceneSphereMaterial(glm::vec4(0.f));
        Renderer::addTexturePack("sceneSphereMaterial", std::move(sceneSphereMaterial));
        Mesh drawSceneSphereMesh = loadUVsphere(24, 24);
        drawSceneSphereMesh.setModelTrans(sceneSphereModel);
        DrawableMeshContainer drawSceneSphere(&drawSceneSphereMesh, "sceneSphereMaterial");
        Renderer::addDrawable(&drawSceneSphere);

        //
        // Eyeball

        //
        //  Lens & Eyeball

        // The "base transform" for the eyeball and lens. Transforms them so that they are in the proper relative positions
        const glm::mat4 eyeballIntermTransform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -2.f)), glm::vec3(2.f * 1.55f));
        const glm::mat4 lensIntermTransform = glm::scale(glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)), glm::vec3(1.f, ARTIFICIAL_EYE_PROP.lens_thickness, 1.f));
        const glm::mat4 pupilInterimTransform = glm::mat4(1.f);

        // Update the model transforms for each of these (NOTE: this is done each frame, with globalModel changing each frame)
        glm::mat4 eyeballModel = globalModel * eyeballIntermTransform;
        glm::mat4 lensModel = globalModel * lensIntermTransform;
        glm::mat4 pupilModel = globalModel * pupilInterimTransform;

        Sphere eyeball(eyeballModel);
        RTSphere rtEyeball(&eyeball, ARTIFICIAL_EYE_PROP.eyeball_refr_index, false);

        Pupil pupil(pupilModel, 1.f);

        Mesh lensMesh = loadUVsphere(ARTIFICIAL_EYE_PROP.longitude, ARTIFICIAL_EYE_PROP.latitude);
        lensMesh.setModelTrans(lensModel);

        RTMesh rtLens(&lensMesh, ARTIFICIAL_EYE_PROP.lens_refr_index, false);
        Lens lens(&lensMesh, ARTIFICIAL_EYE_PROP.latitude, ARTIFICIAL_EYE_PROP.longitude);

        RefractTextPack lensMaterial(glm::vec3(0.f), g_textureDir + g_cubeMapDir, g_cubeMapFaces, ARTIFICIAL_EYE_PROP.lens_refr_index);
        Renderer::addTexturePack("lensMaterial", std::move(lensMaterial));
        DrawableMeshContainer drawLens(&lensMesh, "lensMaterial", true);
        Renderer::addDrawable(&drawLens);

        SBClosedBodySim lensSim(ARTIFICIAL_EYE_PROP.pressure, &lensMesh, ARTIFICIAL_EYE_PROP.mass, ARTIFICIAL_EYE_PROP.extspring_coeff, ARTIFICIAL_EYE_PROP.extspring_drag);
        lensSim.m_constIterations = ARTIFICIAL_EYE_PROP.iterations;
        lensSim.addIntegrator(&ee::SBVerletIntegrator(1.f / 30.f, ARTIFICIAL_EYE_PROP.extspring_drag));
        addInteriorSpringsUVSphere(&lensSim, ARTIFICIAL_EYE_PROP.latitude, ARTIFICIAL_EYE_PROP.longitude, ARTIFICIAL_EYE_PROP.intspring_coeff, ARTIFICIAL_EYE_PROP.intspring_drag);
        g_constraints = lens.addConstraints(5, &lensSim);
        g_allConstraints = lensSim.getConstraints();

        // The amount to increment the counter by:
        const float incHeight = maxHeight / static_cast<float>(res_height);
        const float incWidth = maxWidth / static_cast<float>(res_width);

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
        ImageBuffer imageBuffer(res_width, res_height); // for rendering:
        RayTracer tracer(&pos, &rtLens, &rtEyeball, &rtSceneSphere, &pupil, ARTIFICIAL_EYE_PROP.threads, ARTIFICIAL_EYE_PROP.samples);

        pupil.generateSamples(16);

        // Render the image buffer if required:
#ifndef INVISIBLE
        Renderer::generatePlaneBuffer();
#endif

        tracer.raytraceAll();
        const std::vector<RayTracer::RayPath>& initPaths = tracer.getLines();

        std::vector<DrawLine> drawPaths;
        ee::Renderer::addTexturePack("lineTextPack", ee::LineUniColorTextPack(Vec3(0.0, 1.0, 0.0)));
        for (const RayTracer::RayPath& line : initPaths)
        {
            for (int i = 0; i < 4; i++)
            {
                drawPaths.push_back(DrawLine("lineTextPack", line.lines[i]));
            }
        }

        for (DrawLine& line : drawPaths)
        {
            Renderer::addDrawable(&line);
        }

        for (int frameIndex = 0; frameIndex < g_framePositions.size(); frameIndex++)
        {
            const auto timeNow = std::chrono::high_resolution_clock::now();

            if (!ee::Renderer::isInitialized())
            {
                break;
            }

            const EyePosition currPosition = g_framePositions[frameIndex];
            rotationEye = glm::mat4_cast(currPosition.rotation);
            positionEye = glm::translate(glm::mat4(1), currPosition.position);

            globalModel = positionEye * rotationEye;

            eyeballModel = globalModel * eyeballIntermTransform;
            lensModel = globalModel * lensIntermTransform;
            pupilModel = globalModel * pupilInterimTransform;

            // update the model information:
            lensMesh.setModelTrans(lensModel);
            eyeball.setPosition(eyeballModel);
            pupil.pos = pupilModel;

            lensSim.update(ARTIFICIAL_EYE_PROP.time_step);
            rtLens.updateCache();
            lensMesh.calcNormals();
            tracer.raytraceAll();

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            Renderer::unsetPlaneBufferTexture();

            const auto& paths = tracer.getLines();
            int drawLinesCounter = 0;
            for (int i = 0; i < paths.size() && i < drawPaths.size() / 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    drawPaths[drawLinesCounter].setLine(paths[i].lines[j]);
                }
                drawLinesCounter++;
            }

            const auto timeLater = std::chrono::high_resolution_clock::now();

            captureFrame(frameIndex, tracer, &imageBuffer);

#ifndef INVISIBLE
            //ee::Renderer::setPlaneBufferTexture(imageBuffer.getTextureID());
            Renderer::clearBuffers();
            Renderer::drawAll();
            Renderer::update(Renderer::timeElapsed());
            Renderer::swapBuffers();
            Renderer::pollEvents();
#endif
            const auto timeDiff = timeLater - timeNow;
            std::cout << "Render Duration: " << std::setprecision(5) << static_cast<float>(timeDiff.count()) / 1e9 << " s" << std::endl;
        }

        writeCSVLines(OUTPUT_DIR);
        std::cout << "Wrote result to output." << std::endl;
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