#pragma once

#include <cstddef>
#include <vector>
#include <glm/glm.hpp>
#include <thread>

#include "../Types.hpp"
#include "../Rendering/DrawLine.hpp"
#include "../Mesh/Mesh.hpp"
#include "EyeBall.hpp"
#include "RTUtility.hpp"
#include "RTRay.hpp"
#include "Scene.hpp"
#include "Pupil.hpp"

namespace ee
{
	class RayTracer
	{
	public:
		static RayTracer& initialize(const std::vector<glm::vec3>* pos, const RTObject* lens, const RTObject* eyeball, const Scene* scene,
            Pupil* pupil, int nthreads, int distFactor, int angleFactor, bool drawRays)
		{
			static RayTracer raytracer(pos, lens, eyeball, scene, pupil, nthreads, distFactor, angleFactor, drawRays);
            ms_totalSamples = angleFactor * distFactor;
			return raytracer;
		}

		/*
		* Ray traces the rays. */
		void raytraceAll();

	public:
		struct PhotoReceptor
		{
			glm::vec3 pos;
			glm::vec3 color;
		};

		const std::vector<glm::vec3>& getColors() const
		{
			return m_colors;
		}

	private:
		RayTracer(const std::vector<glm::vec3>* pos, const RTObject* lens, const RTObject* eyeball, const Scene* scene, 
            Pupil* pupil, int nthreads, int distFactor, int angleFactor, bool drawRays);

		void raytraceSelect(int pos, int numrays);
		void raytraceOne(int pos);

        // used for rendering the rays in the scene
        struct IndividualPath
        {
            std::vector<Line> lines;
            mutable int lastValidLine;

            void addLine(Line line)
            {
                lines[lastValidLine] = line;
                lastValidLine++;
            }

            IndividualPath() : lastValidLine(0)
            {
                lines.resize(4 * ms_totalSamples);
            }
        };

        // Ray traces fixed from the eyeball.
        Ray raytraceFromEye(int pos, glm::vec3 pupilPos, IndividualPath* localPaths);

    private:
		std::vector<glm::vec3> m_colors;
        const std::vector<glm::vec3>* m_photoPos;

        const bool m_drawLines;

		const Scene* m_scene;

		const RTObject* const m_lens;
		const RTObject* const m_eyeball;

		const int m_distFactor;
        const int m_angleFactor;
        const int m_totalSamples;

        static int ms_totalSamples;

        Pupil* m_pupil;

		std::vector<std::thread> m_threads;

        std::vector<IndividualPath> m_individualRayPaths;
    };
}