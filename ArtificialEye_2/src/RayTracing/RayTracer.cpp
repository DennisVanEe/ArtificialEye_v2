#include "RayTracer.hpp"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

ee::RayTracer::RayTracer(const std::vector<glm::vec3>* pos, const std::vector<int>* chosen, const RTMesh* lens, const RTSphere* eyeball, const RTSphere* scene,
    const Pupil* pupil, int nthreads, int samples) :
    m_photoPos(pos),
    m_chosenDraw(chosen),
    m_chosenMap(pos->size(), -1),
    m_sampleInv(1.f / static_cast<float>(samples)),
    m_moduloDraw(135),
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

    int counter = 0;
    for (int i : *chosen)
    {
        m_chosenMap[i] = counter;
        counter++;
    }

    m_lines.resize(m_samples * m_chosenDraw->size());
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

const std::vector<float>& ee::RayTracer::getColors() const
{
    return m_colors;
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
    const bool draw = m_chosenMap[photorecpPos] != -1;
    const int drawPosition = m_chosenMap[photorecpPos];

	// first we need to sample a bunch of rays off the circular lens (basically prepare a bunch of paths)
    m_colors[photorecpPos] = 0.f;
    const std::vector<glm::vec3>* const samples = m_pupil->getSamples();
    int samplePos = 0;
	for (glm::vec3 pupilSample : *samples)
	{
        pupilSample = transPoint3(m_pupil->pos, pupilSample);
        const Ray outray = raytraceFromEye(photorecpPos, pupilSample, samplePos, draw);
        if (std::isnan(outray.origin.x))
        {
            continue;
        }

        if (draw)
        {
            // intersect with sphere:
            RTSphereIntersection intersection;
            if (m_sceneSphere->intersect(outray, &intersection))
            {
                m_colors[photorecpPos] += 1.f;
                m_lines[m_samples * drawPosition + samplePos].lines[4] = Line(outray.origin, intersection.point);
            }
            else
            {
                Line line(outray.origin, outray.origin + outray.dir * 100.f);
                m_lines[m_samples * drawPosition + samplePos].lines[4] = line;
            }
        }
        else
        {
            if (m_sceneSphere->intersect(outray))
            {
                m_colors[photorecpPos] += 1.f;
            }
        }
        samplePos++;
	}

	// now we average the values to get the color:
    m_colors[photorecpPos] = m_colors[photorecpPos] * m_sampleInv;
}

ee::Ray ee::RayTracer::raytraceFromEye(int photorecpPos, glm::vec3 pupilPos, int samplePos, bool draw)
{
    const int drawPosition = m_chosenMap[photorecpPos];
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
    glm::vec3 point = ray0.origin;
    point.y *= 0.015f;
    point.x *= 0.015f;
    Line line0(point, intpoint);
    bool aligned0 = glm::dot(dir, glm::vec3(1.f, 0.f, 0.f)) >= 0.f;

	// new we intersect through the lens:
    if (!m_lens->intersect(ray1, ignore, &meshIntersect))
    {
        return NaNRay;
    }
    ignore = meshIntersect.face;
    intpoint = meshIntersect.point;
	dir = glm::normalize(ee::refract(ray1.dir, -meshIntersect.normal, m_lens->getRefraction() / m_eyeball->getRefraction()));
	const Ray ray2(intpoint, dir);
    Line line1(ray1.origin, intpoint);
    bool aligned1 = glm::dot(dir, glm::vec3(1.f, 0.f, 0.f)) >= 0.f;

    RTSphereIntersection sphereIntersect;

	// now we intersect through the cornea (and out into the world!)
    if (!m_eyeball->intersect(ray2, &sphereIntersect))
    {
        return NaNRay;
    }
	intpoint = sphereIntersect.point;
	dir = glm::normalize(ee::refract(ray1.dir, -sphereIntersect.normal, m_eyeball->getRefraction()));
	const Ray ray3(intpoint, dir);
    Line line2(ray2.origin, intpoint);
    bool aligned2 = glm::dot(dir, glm::vec3(1.f, 0.f, 0.f)) >= 0.f;

    if (draw)
    {
        RayPath* path = &m_lines[m_samples * drawPosition + samplePos];
        path->lines[0] = aligned0 ? line0 : Line(glm::vec3(0.f), glm::vec3(0.f));
        path->lines[1] = aligned1 ? line1 : Line(glm::vec3(0.f), glm::vec3(0.f));
        path->lines[2] = aligned2 ? line2 : Line(glm::vec3(0.f), glm::vec3(0.f));
    }

	return ray3;
}