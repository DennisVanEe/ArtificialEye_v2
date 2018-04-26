#include "RayTracer.hpp"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

ee::RayTracer::RayTracer(const std::vector<glm::vec3>* pos, const RTObject* lens, const RTObject* eyeball, const Scene* scene,
    Pupil* pupil, int nthreads, int distFactor, int angleFactor, bool drawRays) :
    m_photoPos(pos),
    m_drawLines(drawRays),
	m_scene(scene),
	m_distFactor(distFactor),
    m_angleFactor(angleFactor),
    m_totalSamples(distFactor * angleFactor),
    m_pupil(pupil),
	m_eyeball(eyeball),
	m_lens(lens)
{
    assert(m_distFactor > 0);
    assert(m_angleFactor > 0);
    assert(nthreads > 0);

    // The total number of possible ray-paths:
    const int totalNumSamples = m_totalSamples * m_photoPos->size();

    // Generate the samples:
    m_pupil->sampleFixed(distFactor, angleFactor);

    // don't allocate if not drawing
    if (m_drawLines)
    {
        m_individualRayPaths.resize(m_photoPos->size());
    }

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
    m_individualRayPaths[photorecpPos].lastValidLine = 0;

	// first we need to sample a bunch of rays off the circular lens (basically prepare a bunch of paths)
	m_colors[photorecpPos] = glm::vec3(0.f);
    const std::vector<glm::vec3>* samples = m_pupil->getFixedSamples();
	for (glm::vec3 pupilSample : *samples)
	{
        int currIndex = m_individualRayPaths[photorecpPos].lastValidLine;
        const Ray outray = raytraceFromEye(photorecpPos, pupilSample, &m_individualRayPaths[photorecpPos]);
        if (std::isnan(outray.origin.x))
        {
            continue;
        }

        // loop over scene:
        int numsceneItems = m_scene->getNumObjects();
        for (int i = 0; i < numsceneItems; i++)
        {
            const RTObject* obj = m_scene->getObject(i);
            if (obj->calcIntersection(outray, -1))
            {
                if (m_drawLines)
                {
                    m_individualRayPaths[photorecpPos].addLine(Line(outray.origin, obj->intPoint()));
                }
                m_colors[photorecpPos] += glm::vec3(1.f, 0.f, 0.f);
            }
        }
	}

	// now we average the values to get the color:
    m_colors[photorecpPos] = m_colors[photorecpPos] / static_cast<float>(m_totalSamples);
}

ee::Ray ee::RayTracer::raytraceFromEye(int photorecpPos, glm::vec3 pupilPos, IndividualPath* localPaths)
{
    const glm::vec3 origin = glm::vec3(m_eyeball->getPosition() *  glm::vec4((*m_photoPos)[photorecpPos], 1.f));
    const Ray ray0(origin, glm::normalize(pupilPos - origin));

	// first we intersect it with the lens:
	bool res = m_lens->calcIntersection(ray0, -1);
    if (!res)
    {
        return Ray(glm::vec3(NaN), glm::vec3(NaN));
    }
	int ignore = m_lens->intFace();
	glm::vec3 intpoint = m_lens->intPoint(); // the point of intersection
	glm::vec3 dir = glm::normalize(ee::refract(ray0.dir, m_lens->intNormalInterpolated(), m_eyeball->refractiveIndex / m_lens->refractiveIndex));
	const Ray ray1(intpoint, dir);

	// new we intersect through the lens:
	res = m_lens->calcIntersection(ray1, ignore);
    if (!res)
    {
        return Ray(glm::vec3(NaN), glm::vec3(NaN));
    }
	ignore = m_lens->intFace();
	intpoint = m_lens->intPoint();
	dir = glm::normalize(ee::refract(ray1.dir, -m_lens->intNormalInterpolated(), m_lens->refractiveIndex / m_eyeball->refractiveIndex));
	const Ray ray2(intpoint, dir);

	// now we intersect through the cornea (and out into the world!)
	res = m_eyeball->calcIntersection(ray2, -1);
    if (!res)
    {
        return Ray(glm::vec3(NaN), glm::vec3(NaN));
    }
	intpoint = m_eyeball->intPoint();
	dir = glm::normalize(ee::refract(ray1.dir, -m_eyeball->intNormalInterpolated(), m_eyeball->refractiveIndex));
	const Ray ray3(intpoint, dir);

    // insert the lins as appropriate:
    if (m_drawLines)
    {
        localPaths->addLine(Line(ray0.origin, ray1.origin));
        localPaths->addLine(Line(ray1.origin, ray2.origin));
        localPaths->addLine(Line(ray2.origin, ray3.origin));
    }

	return ray3;
}