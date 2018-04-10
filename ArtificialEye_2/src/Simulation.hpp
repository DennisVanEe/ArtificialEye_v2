#pragma once

#include "RayTracing/RTObject.hpp"
#include "RayTracing/Scene.hpp"
#include "RayTracing/FrameBuffer.hpp"

namespace ee
{
    struct SimParam
    {
        // Framesbuffer parameters:
        int                nSimFrames;
        FRAMES_BUFFER_MODE colorMode;

        // Raytracer parameters:
        RTObject*          rtLens;
        RTObject*          rtEyeball;
        Scene*             rtScene;
        int                nThreads;
        int                angleFactor;
        int                distFactor;
    };

    bool initializeSimulation(const SimParam& param);
    bool deinitializeSimulation();

    // Sets the muscle tension to be used:
    void setMuslceValue(float muslce);
    // Commits results of frame 
    void pushFrame();

    // Runs the simulation:
    void stepSimulation(float timeDelta);

    const FramesBuffer* getFramesBuffer();
}