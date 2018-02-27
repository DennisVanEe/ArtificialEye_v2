#include "RayTracer.hpp"

ee::RayTracer::RayTracer(const std::vector<glm::vec3>& pos, const RTObject* lens, const RTObject* eyeball, const Scene* scene,
    int maxIterations, int nthreads, int nsamples) :
	m_scene(scene),
	m_sampleCount(nsamples),
    m_maxIterations(maxIterations),
	m_eyeball(eyeball),
	m_lens(lens)
{
    assert(maxIterations > 0);
    assert(nthreads > 0);
    assert(nsamples > 0);

	m_threads.resize(nthreads);
	m_photoReceptors.resize(pos.size());
    for (int i = 0; i < pos.size(); i++)
    {
		m_photoReceptors[i].pos = pos[i];
    }
}

void ee::RayTracer::raytraceAll()
{
	m_raypaths.clear();

	int numPerThreads = m_photoReceptors.size() / m_threads.size();
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
    for (int i = pos, k = 0; k < numrays && i < m_photoReceptors.size(); i++, k++)
    {
        raytraceOne(i);
    }
}

// don't have the lens or eyeball in the scene properties, this
// is to allow for the rays to properly exit efficiently.
void ee::RayTracer::raytraceOne(int photorecpPos)
{
	// first we need to sample a bunch of rays off the circular lens (basically prepare a bunch of paths)
	m_photoReceptors[photorecpPos].color = glm::vec3(0.f);
	for (int i = 0; i < m_sampleCount; i++)
	{
		const Ray outray = raytraceFromEye(photorecpPos);
		if (std::isnan(outray.dir.x))
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
				m_raypaths.push_back(Line(outray.origin, obj->intPoint()));
				m_photoReceptors[photorecpPos].color += glm::vec3(1.f, 0.f, 0.f);
			}
		}
	}

	// now we average the values to get the color:
	m_photoReceptors[photorecpPos].color = m_photoReceptors[photorecpPos].color / static_cast<float>(m_sampleCount);
}

ee::Ray ee::RayTracer::raytraceFromEye(int photorecpPos)
{
    const glm::vec3 origin = glm::vec3(m_eyeball->getPosition() *  glm::vec4(m_photoReceptors[photorecpPos].pos, 1.f));
	const Ray ray0(origin, (glm::vec3(randomSampleUnit(), 0.f) - origin));

	// first we intersect it with the lens:
	bool res = m_lens->calcIntersection(ray0, -1); assert(res); // if this doesn't work, something went horrible wrong
	int ignore = m_lens->intFace();
	glm::vec3 intpoint = m_lens->intPoint(); // the point of intersection
	glm::vec3 dir = glm::normalize(ee::refract(ray0.dir, m_lens->intNormalInterpolated(), m_eyeball->refractiveIndex / m_lens->refractiveIndex));
	const Ray ray1(intpoint, dir);

	if (std::isnan(ray1.dir.x))
	{
		return Ray(glm::vec3(NaN), glm::vec3(NaN));
	}

	m_raypaths.push_back(Line(ray0.origin, ray1.origin));

	// new we intersect through the lens:
	res = m_lens->calcIntersection(ray1, ignore); assert(res);
	ignore = m_lens->intFace();
	intpoint = m_lens->intPoint();
	dir = glm::normalize(ee::refract(ray1.dir, -m_lens->intNormalInterpolated(), m_lens->refractiveIndex / m_eyeball->refractiveIndex));
	const Ray ray2(intpoint, dir);

	m_raypaths.push_back(Line(ray1.origin, ray2.origin));

	if (std::isnan(ray2.dir.x))
	{
		return Ray(glm::vec3(NaN), glm::vec3(NaN));
	}

	// now we intersect through the cornea (and out into the world!)
	res = m_eyeball->calcIntersection(ray2, -1); assert(res);
	intpoint = m_eyeball->intPoint();
	dir =  glm::normalize(ee::refract(ray1.dir, -m_eyeball->intNormalInterpolated(), m_eyeball->refractiveIndex));
	const Ray ray3(intpoint, dir);

	if (std::isnan(ray3.dir.x))
	{
		return Ray(glm::vec3(NaN), glm::vec3(NaN));
	}

	m_raypaths.push_back(Line(ray2.origin, ray3.origin));
	//m_raypaths.push_back(Line(ray3.origin, ray3.origin + (ray3.dir) * 10.f));

	return ray3;
}