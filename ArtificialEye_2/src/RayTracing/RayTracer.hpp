#pragma once

#include <cstddef>
#include <vector>
#include <glm/glm.hpp>
#include <thread>

#include "../Mesh/Mesh.hpp"
#include "EyeBall.hpp"
#include "../Types.hpp"
#include "../Rendering/DrawLine.hpp"
#include "RTUtility.hpp"
#include "RTRay.hpp"
#include "Scene.hpp"
#include "FrameBuffer.hpp"

namespace ee
{
	class RayTracer
	{
	public:
		static RayTracer& initialize(const glm::vec3* positions, int nPositions, FramesBuffer* framesBuffer, const RTObject* lens, const RTObject* eyeball, const Scene* scene,
            int nthreads, int distFactor, int angleFactor)
		{
			static RayTracer raytracer(positions, nPositions, framesBuffer, lens, eyeball, scene, nthreads, distFactor, angleFactor);
			return raytracer;
		}

        ~RayTracer();

        const Line* getRayPaths() const
        {
            return m_rayPaths;
        }

        int getNumRayPaths() const
        {
            return m_nRayPaths;
        }

		/*
		* Ray traces the rays. */
		void raytraceAll();

        void stepFrame()
        {
            m_currentFrame++;
        }

	private:
		RayTracer(const glm::vec3* positions, int nPositions, FramesBuffer* framesBuffer, const RTObject* lens, const RTObject* eyeball, const Scene* scene, int nthreads, int distFactor, int angleFactor);

		void raytraceSelect(int pos, int numrays);
		void raytraceOne(int pos);

        // Ray traces fixed from the eyeball.
        Ray raytraceFromEye(int pos, int dist, int angle, Line* localPaths, int* freePosition);

    private:
		const glm::vec3* m_photoReceptors;
        const int m_nPhotoReceptors;

        int m_currentFrame;
        FramesBuffer* m_framesBuffer;

		const Scene* m_scene;
		const RTObject* const m_lens;
		const RTObject* const m_eyeball;

		const int m_distFactor;
        const int m_angleFactor;
        const int m_totalSamples;

		std::thread* m_threads;
        int m_nThreads;

        Line* m_rayPaths;
        int m_nRayPaths;
        int m_linesPerPhotoReceptor;
    };
}