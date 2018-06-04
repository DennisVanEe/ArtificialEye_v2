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
        RayTracer(const std::vector<glm::vec3>* pos, const std::vector<int>& drawnSamples, const RTMesh* lens, const RTSphere* eyeball, const RTSphere* sceneSphere,
            const Pupil* pupil, int nthreads, int samples);

		void raytraceAll();
        const std::vector<float>& getColors() const;
        const std::vector<Path>& getPath() const
        {
            return m_paths;
        }

	private:

		void raytraceSelect(int pos, int numrays);
		__forceinline void raytraceOne(int pos);

        // Ray traces fixed from the eyeball.
        __forceinline Ray raytraceFromEye(int pos, glm::vec3 pupilPos, int samplePos, bool addLines);

    private:
		std::vector<float> m_colors;
        const std::vector<glm::vec3>* m_photoPos;

        std::unordered_map<int, int> m_drawableRays;

        const float m_sampleInv;

        const Pupil* const m_pupil;

		const RTMesh*   const m_lens;
		const RTSphere* const m_eyeball;
        const RTSphere* const m_sceneSphere;

        const int m_samples;

        std::vector<Path> m_paths;

		std::vector<std::thread> m_threads;
    };
}