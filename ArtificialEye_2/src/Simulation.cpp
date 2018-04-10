#include "Simulation.hpp"

#include <glm/vec3.hpp>
#include "RayTracing/rayTracer.hpp"

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
    bool             initialized         = false;

    // ray tracer information:
    ee::RayTracer*   g_raytracer         = nullptr;
    const glm::vec3* g_photoReceptors    = nullptr;
    float*           g_muscleConstraints = nullptr;
    FramesBuffer     g_framesBuffer;
    int              g_currentFrame      = 0;

    // simulation information:
    float            g_constraint;
}

bool ee::initializeSimulation(const SimParam& param)
{
    if (initialized)
    {
        return false;
    }

    // Generate the photoreceptors
    int nPhotoreceptors;
    g_photoReceptors = generatePhotoreceptors(&nPhotoreceptors);
    
    // Prepare the frames buffer
    g_framesBuffer.initialize(nPhotoreceptors, param.nSimFrames, param.colorMode);

    g_raytracer = &ee::RayTracer::initialize(g_photoReceptors, nPhotoreceptors, &g_framesBuffer, param.rtLens, param.rtEyeball, 
        param.rtScene, param.nThreads, param.distFactor, param.angleFactor);



    return true;
}

bool ee::deinitializeSimulation()
{
    delete[] g_photoReceptors;
    initialized = false;

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

}

const ee::FramesBuffer* ee::getFramesBuffer()
{
    return &g_framesBuffer;
}
