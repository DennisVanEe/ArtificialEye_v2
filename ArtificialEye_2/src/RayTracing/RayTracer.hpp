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

namespace ee
{
	class RayTracer
	{
	public:
		static RayTracer& initialize(const std::vector<glm::vec3>& pos, const EyeBall* eyeball, const Scene* scene, 
            int maxIterations, int nthreads, int nsamples)
		{
			static RayTracer raytracer(pos, eyeball, scene, maxIterations, nthreads, nsamples);
			return raytracer;
		}

		/*
		* Ray traces the rays. */
		void raytraceAll();

        const std::vector<Line>& getRayPaths() const
        {
            return m_raypaths;
        }

        std::vector<Line>& getRayPaths()
        {
            return m_raypaths;
        }

	private:
		struct PhotoReceptor
		{
			glm::vec3 pos;
			glm::vec3 color;
		};

		RayTracer(const std::vector<glm::vec3>& pos, const EyeBall* eyeball, const Scene* scene, int maxIterations, int nthreads, int nsamples);

		void raytraceSelect(int pos, int numrays);
		void raytraceOne(int pos);

        // Actual raytracing implementation:
        const RTObject* intersectRay(RTRay ray, const RTObject* objIgnore, int triangleIgnore) const;
        const RTObject* raytrace(RTRay ray);

    private:
		std::vector<PhotoReceptor> m_photoReceptors;

		const Scene* m_scene;
		const EyeBall* m_eyeball;

		const int m_sampleCount;
        const int m_maxIterations;

		std::vector<std::thread> m_threads;

		std::vector<Line> m_raypaths; // used for rendering the rays in the scene
    };
}