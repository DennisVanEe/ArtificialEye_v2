#pragma once

#include "RayTracing/RTObject.hpp"
#include "RayTracing/Scene.hpp"
#include "RayTracing/FrameBuffer.hpp"
#include "Initialization.hpp"

namespace ee
{
    struct SimParam
    {
        // Framesbuffer parameters:
        int                    nSimFrames;
        FRAMES_BUFFER_MODE     colorMode;

        // Raytracer parameters:
        Scene*                 rtScene;
        int                    nThreads;
        int                    angleFactor;
        int                    distFactor;

        // Render parameters:
        RendererParam*         renderParam;
        CameraParam*           cameraParams;
    };

    bool initializeSimulation(const SimParam& param);
    bool deinitializeSimulation();

    // Sets the muscle tension to be used:
    void setMuslceValue(float muslce);
    // Commits results of frame 
    void pushFrame();

    // For drawing:
    

    // Runs the simulation:
    void stepSimulation(float timeDelta);

    const FramesBuffer* getFramesBuffer();
}