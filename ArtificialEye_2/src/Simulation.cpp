#include "Simulation.hpp"

#include <glm/vec3.hpp>
#include "RayTracing/rayTracer.hpp"
#include "SoftBody/Simulation/SBClosedBodySim.hpp"
#include "SoftBody/SBUtilities.hpp"
#include "SoftBody/Integrators/SBVerletIntegrator.hpp"
#include "RayTracing/EyeBall.hpp"
#include "Rendering/Lens.hpp"
#include "Mesh/MeshGenerator.hpp"
#include "Mesh/Subdivision.hpp"

const glm::vec3* generatePhotoreceptors(int* nPhotoreceptors)
{
    // Temporary parameters for the placeholder positions:
    static const float maxHeight  = 1.0f;
    static const float maxWidth   = 1.0f;
    static const int   res_width  = 16;
    static const int   res_height = 16;
    static const float incHeight  = maxHeight / static_cast<float>(res_height);
    static const float incWidth   = maxWidth  / static_cast<float>(res_width);
    static const int   numPhotoreceptorsPosition = res_width * res_height;

    *nPhotoreceptors = numPhotoreceptorsPosition;
        
    glm::vec3* photoreceptorPositions = new glm::vec3[numPhotoreceptorsPosition];

    int photoIndex = 0;

    float width = -maxWidth / 2.f;
    for (int w = 0; w < res_width; w++, width += incWidth)
    {
        float height = -maxHeight / 2.f;
        for (int h = 0; h < res_height; h++, height += incHeight)
        {
            photoreceptorPositions[photoIndex] = glm::vec3(width, height, -1.f);
        }
    }

    return photoreceptorPositions;
}

// Simulation properties:
namespace ee
{
    bool                                g_initialized       = false;

    // ray tracer information:
    ee::RayTracer*                      g_raytracer         = nullptr;
    const glm::vec3*                    g_photoReceptors    = nullptr;
    float*                              g_muscleConstraints = nullptr;
    FramesBuffer                        g_framesBuffer;
    int                                 g_currentFrame      = 0;

    // simulation information:
    float                               g_constraint;
    SBClosedBodySim*                    g_simulation;
    Lens*                               g_lens;
    RTObjectSphere*                     g_rtEyeball;
    RTObjectMesh*                       g_rtLens;

    std::vector<ee::SBPointConstraint*> g_constraints;
    Mesh                                g_simLensMesh;
    Mesh                                g_traceLensMesh;
}

bool ee::initializeSimulation(const SimParam& param)
{
    if (g_initialized)
    {
        return false;
    }

    // Now prepare the scene and all the objects for the ray tracer:

    glm::mat4 eyeballTransform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -2.f));
    eyeballTransform = glm::scale(eyeballTransform, glm::vec3(2 * 1.55f, 2 * 1.55f, 2 * 1.55f));

    glm::mat4 lensTransform = glm::scale(glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)), glm::vec3(1.f, ARTIFICIAL_EYE_PROP.lens_thickness, 1.f));

    // Generate the required the meshes:
    g_simLensMesh = loadUVsphere(ARTIFICIAL_EYE_PROP.longitude, ARTIFICIAL_EYE_PROP.latitude);
    g_simLensMesh.setModelTrans(lensTransform);
    g_traceLensMesh = loopSubdiv(g_simLensMesh, ARTIFICIAL_EYE_PROP.subdiv_level_lens);
    g_traceLensMesh.calcNormals();
    g_traceLensMesh.setModelTrans(lensTransform);

    // Now setup the actual simulation:
    g_simulation = new SBClosedBodySim(ARTIFICIAL_EYE_PROP.pressure, &g_simLensMesh, ARTIFICIAL_EYE_PROP.mass, ARTIFICIAL_EYE_PROP.extspring_coeff, ARTIFICIAL_EYE_PROP.extspring_drag);
    addInteriorSpringsUVSphere(g_simulation, ARTIFICIAL_EYE_PROP.latitude, ARTIFICIAL_EYE_PROP.longitude, ARTIFICIAL_EYE_PROP.intspring_coeff, ARTIFICIAL_EYE_PROP.intspring_drag);
    g_simulation->addIntegrator(&ee::SBVerletIntegrator(1.f / 20.f, ARTIFICIAL_EYE_PROP.extspring_drag));

    // Generate the simulation information:
    g_rtLens = new RTObjectMesh("lens", &g_traceLensMesh, ARTIFICIAL_EYE_PROP.lens_refr_index);
    g_rtEyeball = new RTObjectSphere("eyeball", eyeballTransform, ARTIFICIAL_EYE_PROP.eyeball_refr_index);

    // Generate the lens based on the lens mesh:
    g_lens = new Lens(&g_traceLensMesh, ARTIFICIAL_EYE_PROP.latitude, ARTIFICIAL_EYE_PROP.longitude);
    g_constraints = g_lens->addConstraints(5, g_simulation);

    // Generate the photoreceptors
    int nPhotoreceptors;
    g_photoReceptors = generatePhotoreceptors(&nPhotoreceptors);
    
    // Prepare the frames buffer
    g_framesBuffer.initialize(nPhotoreceptors, param.nSimFrames, param.colorMode);

    // Prepare the ray tracer:
    g_raytracer = &ee::RayTracer::initialize(g_photoReceptors, nPhotoreceptors, &g_framesBuffer, g_rtLens, g_rtEyeball, param.rtScene, param.nThreads, param.distFactor, param.angleFactor);

    g_initialized = true;

    return true;
}

bool ee::deinitializeSimulation()
{
    delete[] g_photoReceptors;
    delete[] g_muscleConstraints;
    delete g_simulation;
    delete g_lens;
    delete g_rtEyeball;
    delete g_rtLens;

    g_initialized = false;

    return true;
}

void ee::setMuslceValue(float muslce)
{
    g_constraint = muslce;
}

void ee::pushFrame()
{
    g_muscleConstraints[g_currentFrame++] = g_constraint;
    g_raytracer->stepFrame();
}

void ee::stepSimulation(float timeDelta)
{
    // Set the constraint:
    for (auto& constraint : g_constraints)
    {
        constraint->m_point += g_constraint * glm::normalize(glm::vec3(constraint->m_point.x, 0.f, constraint->m_point.z));
    }

    // Update the simulation and raytraced values
    g_simulation->update(timeDelta);
    g_traceLensMesh = loopSubdiv(g_simLensMesh, ARTIFICIAL_EYE_PROP.subdiv_level_lens);

    // Now we raytrace through the simulation
    g_raytracer->raytraceAll();
}

const ee::FramesBuffer* ee::getFramesBuffer()
{
    return &g_framesBuffer;
}
