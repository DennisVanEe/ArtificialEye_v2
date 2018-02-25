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
	int numThreads = m_photoReceptors.size() / m_threads.size();
	numThreads = numThreads == 0 ? 1 : numThreads;

    for (int i = 0, pos = 0; i < m_photoReceptors.size(); i++, pos += numThreads)
    {
        //m_threads.push_back(std::thread(&RayTracer::raytraceSelect, this, pos, numThreads));
		raytraceSelect(pos, numThreads);
    }

    //for (auto& thread : m_threads)
    //{
    //    thread.join();
    //}
}

void ee::RayTracer::raytraceSelect(int pos, int numrays)
{
    for (int i = pos; i < numrays && i < m_photoReceptors.size(); i++)
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

		// loop over scene:
		int numsceneItems = m_scene->getNumObjects();
		for (int i = 0; i < numsceneItems; i++)
		{
			const RTObject* obj = m_scene->getObject(i);
			if (obj->calcIntersection(outray, -1))
			{
				m_photoReceptors[photorecpPos].color = glm::vec3(1.f, 0.f, 0.f);
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
		return Ray(glm::vec3(0.f), glm::vec3(0.f));
	}

	// now we intersect through the cornea (and out into the world!)
	res = m_eyeball->calcIntersection(ray2, -1); assert(res);
	intpoint = m_eyeball->intPoint();
	dir =  glm::normalize(ee::refract(ray1.dir, m_eyeball->intNormalInterpolated(), m_eyeball->refractiveIndex));
	const Ray ray3(intpoint, dir);

	m_raypaths.push_back(Line(ray2.origin, ray3.origin));

	return Ray(glm::vec3(0.f), glm::vec3(0.f));
}

const ee::RTObject* ee::RayTracer::intersectRay(RTRay ray, const RTObject* objIgnore, int triangleIgnore) const
{
    struct
    {
        const RTObject* value;
        float distance;
    } minInt;

    minInt.value = nullptr;
    minInt.distance = std::numeric_limits<float>::infinity();

    for (int i = 0; i < m_scene->getNumObjects(); i++)
    {
        const RTObject* obj = m_scene->getObject(i);

        const int ignore = obj == objIgnore ? triangleIgnore : -1;
        if (obj->calcIntersection(ray, ignore))
        {
            // get the distance:
            const glm::vec3 distRay = ray.origin - obj->intPoint();
            const float distance = glm::dot(distRay, distRay);
            if (distance < minInt.distance)
            {
                minInt.distance = distance;
                minInt.value = obj;
            }
        }
    }

    return minInt.value;
}

const ee::RTObject* ee::RayTracer::raytrace(RTRay ray)
{
    const RTObject* intersectedObject = nullptr;
    int intersectedTriangle = -1;

    // can't have the ray bouncing forever
    for (int i = 0; i < m_maxIterations; i++)
    {
        Line line;
        line.start = ray.origin; // the line starts out with an initial position
        line.end = glm::vec3(0.f, 0.f, 0.f);

        // First we intersect the ray with an object:
        // This will return to us the type of object.
        intersectedObject = intersectRay(ray, intersectedObject, intersectedTriangle);
        if (intersectedObject == nullptr)
        {
            m_raypaths.push_back(line);
            return nullptr;
        }

        intersectedTriangle = intersectedObject->intFace();

        // Now we look at the object's properties to see if we go through
        // the object or if we leave the object.
        if (intersectedObject->refractiveIndex == 0.0) // the object doesn't support refraction
        {
            if (intersectedObject->isReflective)
            {
                ray.dir = glm::reflect(ray.dir, intersectedObject->intNormalInterpolated());
                ray.origin = intersectedObject->intPoint();
                line.end = intersectedObject->intPoint();
            }
            else
            {
                line.end = intersectedObject->intPoint();
                m_raypaths.push_back(line);
                return intersectedObject; // the light is just absorbed here anyways
            }
        }
        else
        {
            const float currEnvRefraction = ray.objects.size() == 0 ? 1 : ray.objects.top()->refractiveIndex;

            const float entEnvRefraction = intersectedObject->refractiveIndex;
            const float ratio = currEnvRefraction / entEnvRefraction;
            ray.dir = ee::refract(ray.dir, intersectedObject->intNormalInterpolated(), ratio);
            line.end = intersectedObject->intPoint();
            m_raypaths.push_back(line);
            if (ray.dir == glm::vec3(0.f, 0.f, 0.f))
            {
                return intersectedObject; // we didn't intersect the object correctly
            }
            ray.origin = intersectedObject->intPoint();
            ray.objects.push(intersectedObject);
        }
    }

    return intersectedObject; // if we must end due to reaching the iteration maximum.
}