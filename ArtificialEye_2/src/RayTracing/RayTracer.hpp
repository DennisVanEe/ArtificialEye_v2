#pragma once

#include <cstddef>
#include <vector>
#include <glm/glm.hpp>
#include <thread>

#include "../Types.hpp"
#include "../Rendering/DrawLine.hpp"
#include "../Mesh/Mesh.hpp"
#include "RTMesh.hpp"
#include "RTSphere.hpp"
#include "RTUtility.hpp"
#include "Pupil.hpp"

namespace ee
{
	class RayTracer
	{
	public:
        RayTracer(const std::vector<glm::vec3>* pos, const RTMesh* lens, const RTSphere* eyeball, const RTSphere* sceneSphere,
            const Pupil* pupil, int nthreads, int samples);

		void raytraceAll();
        const std::vector<float>* getColors() const;

	private:
		void raytraceSelect(int pos, int numrays);
		void raytraceOne(int pos);

        // Ray traces fixed from the eyeball.
        Ray raytraceFromEye(int pos, glm::vec3 pupilPos);

    private:
		std::vector<float> m_colors;
        const std::vector<glm::vec3>* m_photoPos;

        const Pupil* const m_pupil;

		const RTMesh*   const m_lens;
		const RTSphere* const m_eyeball;
        const RTSphere* const m_sceneSphere;

        const int m_samples;

		std::vector<std::thread> m_threads;
    };
}