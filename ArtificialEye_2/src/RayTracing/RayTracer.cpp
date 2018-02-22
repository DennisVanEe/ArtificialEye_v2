#include "RayTracer.hpp"

ee::RayTracer::RayTracer(const std::vector<glm::vec3>& pos, const EyeBall* eyeball, const Scene* scene, 
    int maxIterations, int nthreads, int nsamples) :
	m_scene(scene),
	m_sampleCount(nsamples),
    m_maxIterations(maxIterations),
	m_eyeball(eyeball)
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

//void ee::RayTracer::generateUDP()
//{
//	static const char pathname[] = "raypath";
//
//	// All the objects in the scene:
//	for (int i = 0; i < m_scene->getNumObjects(); i++)
//	{
//		m_udp.push_back(m_scene->getObject(i)->getUDP());
//	}
//
//	// All the lines in the scene:
//	for (const auto &paths : m_raypaths)
//	{
//		for (const auto &line : paths)
//		{
//			const float *rawdata = &line.start.x;
//			m_udp.push_back(UDP(pathname, UDP_TYPE_LINE, Mat4(), rawdata, 6));
//		}
//	}
//}

void ee::RayTracer::raytraceAll()
{
	int numThreads = m_photoReceptors.size() / m_threads.size();
	numThreads = numThreads == 0 ? 1 : numThreads;

    for (int i = 0, pos = 0; i < m_photoReceptors.size(); i++, pos += numThreads)
    {
        m_threads.push_back(std::thread(&RayTracer::raytraceSelect, this, pos, numThreads));
    }

    for (auto& thread : m_threads)
    {
        thread.join();
    }
}

void ee::RayTracer::raytraceSelect(int pos, int numrays)
{
    for (int i = pos; i < numrays && i < m_photoReceptors.size(); i++)
    {
        raytraceOne(i);
    }
}

void ee::RayTracer::raytraceOne(const int photorecpPos)
{
	// each thread has one of these:
	const glm::vec4 homogOrigin = m_eyeball->m_position *  glm::vec4(m_photoReceptors[photorecpPos].pos, 1.f);
	const glm::vec3 globalOrigin = glm::vec3(homogOrigin);

	// first we need to sample a bunch of rays off the circular lens (basically prepare a bunch of paths)
	for (int i = 0; i < m_sampleCount; i++)
	{
		const glm::vec2 pos = randomSampleUnit();
		const glm::vec4 homogFinalPos = m_eyeball->m_lens->getNormalModelTrans() * glm::vec4(pos, 0.f, 1.f);
		const glm::vec3 dirPos = glm::vec3(homogFinalPos);

		ee::RTRay ray(globalOrigin, glm::normalize(dirPos - globalOrigin));

		const RTObject* object = raytrace(ray);

		// Here goes the super advanced shader...
		if (object != nullptr)
		{
			m_photoReceptors[photorecpPos].color = Vec3(1.f, 0.f, 0.f);
		}
	}

	// now we average the values to get the color:
	m_photoReceptors[photorecpPos].color = m_photoReceptors[photorecpPos].color / static_cast<float>(m_sampleCount);
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

        int ignore = obj == objIgnore ? triangleIgnore : -1;
        if (obj->calcIntersection(ray, ignore))
        {
            // get the distance:
            glm::vec3 distRay = ray.origin - obj->intPoint();
            float distance = glm::dot(distRay, distRay);
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
        if (intersectedObject->refracIndex == 0.0) // the object doesn't support refraction
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
            float currEnvRefraction = ray.objects.size() == 0 ? 1 : ray.objects.top()->refracIndex;

            float entEnvRefraction = intersectedObject->refracIndex;
            float ratio = currEnvRefraction / entEnvRefraction;
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