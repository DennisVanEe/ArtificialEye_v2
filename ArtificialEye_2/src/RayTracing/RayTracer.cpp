#include "RayTracer.hpp"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

ee::RayTracer::RayTracer(const std::vector<glm::vec3>* pos, const RTMesh* lens, const RTSphere* eyeball, const RTSphere* scene,
    const Pupil* pupil, int nthreads, int samples) :
    m_photoPos(pos),
    m_pupil(pupil),
    m_lens(lens),
	m_eyeball(eyeball),
    m_sceneSphere(scene),
    m_samples(samples)
{
    assert(samples > 0);
    assert(nthreads > 0);

    // The total number of possible ray-paths:
    const int totalNumSamples = m_samples * m_photoPos->size();

	m_threads.resize(nthreads);
	m_colors.resize(m_photoPos->size());
}

void ee::RayTracer::raytraceAll()
{
	int numPerThreads = m_photoPos->size() / m_threads.size();
	numPerThreads = numPerThreads == 0 ? 1 : numPerThreads;

    for (int i = 0, pos = 0; i < m_threads.size(); i++, pos += numPerThreads)
    {
		std::thread th(&RayTracer::raytraceSelect, this, pos, numPerThreads);
		m_threads[i] = std::move(th);
    }

    for (auto& thread : m_threads)
    {
        thread.join();
    }
}

const std::vector<float>* ee::RayTracer::getColors() const
{
    return &m_colors;
}

void ee::RayTracer::raytraceSelect(int pos, int numrays)
{
    for (int i = pos, k = 0; k < numrays && i < m_photoPos->size(); i++, k++)
    {
        raytraceOne(i);
    }
}

// don't have the lens or eyeball in the scene properties, this
// is to allow for the rays to properly exit efficiently.
// this is run in one thread
void ee::RayTracer::raytraceOne(int photorecpPos)
{
	// first we need to sample a bunch of rays off the circular lens (basically prepare a bunch of paths)
    m_colors[photorecpPos] = 0.f;
    const std::vector<glm::vec3>* const samples = m_pupil->getSamples();
	for (glm::vec3 pupilSample : *samples)
	{
        pupilSample = transPoint3(m_pupil->pos, pupilSample);
        const Ray outray = raytraceFromEye(photorecpPos, pupilSample);
        if (std::isnan(outray.origin.x))
        {
            continue;
        }

        // intersect with sphere:
        if (m_sceneSphere->intersect(outray))
        {
            m_colors[photorecpPos] += 1.f;
        }
	}

	// now we average the values to get the color:
    m_colors[photorecpPos] = m_colors[photorecpPos] / static_cast<float>(m_samples);
}

ee::Ray ee::RayTracer::raytraceFromEye(int photorecpPos, glm::vec3 pupilPos)
{
    static const Ray NaNRay = Ray(glm::vec3(NaN), glm::vec3(NaN));

    const glm::vec3 origin = transPoint3(m_eyeball->getSphere()->getPosition(), (*m_photoPos)[photorecpPos]);
    const Ray ray0(origin, glm::normalize(pupilPos - origin));

    RTMeshIntersection meshIntersect;

	// first we intersect it with the lens:
    if (!m_lens->intersect(ray0, -1, &meshIntersect))
    {
        return NaNRay;
    }
	int ignore = meshIntersect.face;
	glm::vec3 intpoint = meshIntersect.point; // the point of intersection
	glm::vec3 dir = glm::normalize(ee::refract(ray0.dir, meshIntersect.normal, m_eyeball->getRefraction() / m_lens->getRefraction()));
	const Ray ray1(intpoint, dir);

	// new we intersect through the lens:
    if (!m_lens->intersect(ray1, ignore, &meshIntersect))
    {
        return NaNRay;
    }
    ignore = meshIntersect.face;
    intpoint = meshIntersect.point;
	dir = glm::normalize(ee::refract(ray1.dir, -meshIntersect.normal, m_lens->getRefraction() / m_eyeball->getRefraction()));
	const Ray ray2(intpoint, dir);

    RTSphereIntersection sphereIntersect;

	// now we intersect through the cornea (and out into the world!)
    if (!m_eyeball->intersect(ray2, &sphereIntersect))
    {
        return NaNRay;
    }
	intpoint = sphereIntersect.point;
	dir = glm::normalize(ee::refract(ray1.dir, -sphereIntersect.normal, m_eyeball->getRefraction()));
	const Ray ray3(intpoint, dir);

	return ray3;
}