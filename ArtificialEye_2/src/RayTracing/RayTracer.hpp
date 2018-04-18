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
		static RayTracer& initialize(const std::vector<glm::vec3>& pos, const RTObject* lens, const RTObject* eyeball, const Scene* scene,
            int nthreads, int distFactor, int angleFactor)
		{
			static RayTracer raytracer(pos, lens, eyeball, scene, nthreads, distFactor, angleFactor);
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

	public:
		struct PhotoReceptor
		{
			glm::vec3 pos;
			glm::vec3 color;
		};

		const std::vector<PhotoReceptor>& getPhotoreceptors() const
		{
			return m_photoReceptors;
		}

	private:
		RayTracer(const std::vector<glm::vec3>& pos, const RTObject* lens, const RTObject* eyeball, const Scene* scene, int nthreads, int distFactor, int angleFactor);

		void raytraceSelect(int pos, int numrays);
		void raytraceOne(int pos);

        // Ray traces fixed from the eyeball.
        Ray raytraceFromEye(int pos, int dist, int angle, std::vector<Line>* localPaths);

    private:
		std::vector<PhotoReceptor> m_photoReceptors;

		const Scene* m_scene;

		const RTObject* const m_lens;
		const RTObject* const m_eyeball;

		const int m_distFactor;
        const int m_angleFactor;
        const int m_totalSamples;

		std::vector<std::thread> m_threads;

		std::vector<Line> m_raypaths; // used for rendering the rays in the scene
        std::vector<std::vector<Line>> m_individualRayPaths;
    };
}