#include "RayTracer.hpp"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

#define MAX_NUM_LINES_PER_PATH 4
#define NUM_LINES_IN_EYE 3
#define NUM_LINES_OUTSIDE_EYE 1

ee::RayTracer::RayTracer(const glm::vec3* positions, int nPositions, FramesBuffer* framesBuffer, const RTObject* lens, const RTObject* eyeball, const Scene* scene,
    int nthreads, int distFactor, int angleFactor) :
    m_photoReceptors(positions),
    m_nPhotoReceptors(nPositions),

    m_currentFrame(0),
    m_framesBuffer(framesBuffer),

    m_scene(scene),
    m_eyeball(eyeball),
    m_lens(lens),

	m_distFactor(distFactor),
    m_angleFactor(angleFactor),
    m_totalSamples(distFactor * angleFactor),

    m_nThreads(nthreads)
{
    assert(framesBuffer->getMode() == FRAMES_BUFFER_MODE::FLOAT1);

    m_threads = new std::thread[m_nThreads];
    m_linesPerPhotoReceptor = MAX_NUM_LINES_PER_PATH * m_distFactor * m_angleFactor;
    m_nRayPaths = nPositions * m_linesPerPhotoReceptor;
    m_rayPaths = new Line[m_nRayPaths];
}

ee::RayTracer::~RayTracer()
{
    delete[] m_threads;
    delete[] m_rayPaths;
}

void ee::RayTracer::raytraceAll()
{
    int numPerThreads = m_nPhotoReceptors / m_nThreads;
	numPerThreads = numPerThreads == 0 ? 1 : numPerThreads;

    for (int i = 0, pos = 0; i < m_nThreads; i++, pos += numPerThreads)
    {
        m_threads[i] = std::thread(&RayTracer::raytraceSelect, this, pos, numPerThreads);
    }

    for (int i = 0; i < m_nThreads; i++)
    {
        m_threads[i].join();
    }
}

void ee::RayTracer::raytraceSelect(int pos, int numrays)
{
    for (int i = pos, k = 0; k < numrays && i < m_nPhotoReceptors; i++, k++)
    {
        raytraceOne(i);
    }
}

// don't have the lens or eyeball in the scene properties, this
// is to allow for the rays to properly exit efficiently.
// this is run in one thread
void ee::RayTracer::raytraceOne(int photorecpPos)
{
    const int raypathOffset = photorecpPos * m_linesPerPhotoReceptor;
    Line* allIndividualPaths = &m_rayPaths[raypathOffset]; // all the paths sampled from this photoreceptor
    int globalPosition = 0;

	// first we need to sample a bunch of rays off the circular lens (basically prepare a bunch of paths)
    // glm::vec3 localColor(0.f); - This is when color is introduced
    float localColor = 0.f;
	for (int d = 0; d < m_distFactor; d++)
	{
        for (int a = 0; a < m_angleFactor; a++)
        {
            Line* individualPath = &allIndividualPaths[globalPosition];

            int localPosition;
            const Ray outray = raytraceFromEye(photorecpPos, d, a, individualPath, &localPosition);
            if (std::isnan(outray.dir.x))
            {
                continue;
            }

            globalPosition += localPosition;

            // loop over scene:
            for (int i = 0; i < m_scene->getNumObjects() && i < NUM_LINES_OUTSIDE_EYE; i++)
            {
                const RTObject* obj = m_scene->getObject(i);
                // If there was an intersection
                if (obj->calcIntersection(outray, -1))
                {
                    individualPath[globalPosition++] = Line(outray.origin, obj->intPoint());
                    localColor += 1.f;
                }
            }
        }
	}

	// Now we average the values to get the color:
	localColor = localColor / static_cast<float>(m_totalSamples);
    m_framesBuffer->setPixel(m_currentFrame, photorecpPos, &localColor);
}

ee::Ray ee::RayTracer::raytraceFromEye(int photorecpPos, int dist, int angle, Line* localPaths, int* freePosition)
{
    int currentPosition = 0;

    const glm::vec3 origin = glm::vec3(m_eyeball->getPosition() *  glm::vec4(m_photoReceptors[photorecpPos], 1.f));
    const glm::vec2 sampledPoint = sampleUnit(dist, angle, m_distFactor, m_angleFactor);
	const Ray ray0(origin, glm::normalize(glm::vec3(sampledPoint, 0.f) - origin));

	// first we intersect it with the lens:
	bool res = m_lens->calcIntersection(ray0, -1);
    if (!res)
    {
        return Ray(NaN);
    }
	int ignore = m_lens->intFace();
	glm::vec3 intpoint = m_lens->intPoint(); // the point of intersection
	glm::vec3 dir = glm::normalize(ee::refract(ray0.dir, m_lens->intNormalInterpolated(), m_eyeball->refractiveIndex / m_lens->refractiveIndex));
	const Ray ray1(intpoint, dir);

	if (std::isnan(ray1.dir.x))
	{
        *freePosition = currentPosition;
		return Ray(NaN);
	}

    localPaths[currentPosition++] = Line(ray0.origin, ray1.origin);

	// new we intersect through the lens:
	res = m_lens->calcIntersection(ray1, ignore);
    if (!res)
    {
        *freePosition = currentPosition;
        return Ray(NaN);
    }
	ignore = m_lens->intFace();
	intpoint = m_lens->intPoint();
	dir = glm::normalize(ee::refract(ray1.dir, -m_lens->intNormalInterpolated(), m_lens->refractiveIndex / m_eyeball->refractiveIndex));
	const Ray ray2(intpoint, dir);

    localPaths[currentPosition++] = Line(ray1.origin, ray2.origin);

	if (std::isnan(ray2.dir.x))
	{
        *freePosition = currentPosition;
		return Ray(NaN);
	}

	// now we intersect through the cornea (and out into the world!)
	res = m_eyeball->calcIntersection(ray2, -1);
    if (!res)
    {
        *freePosition = currentPosition;
        return Ray(NaN);
    }
	intpoint = m_eyeball->intPoint();
	dir =  glm::normalize(ee::refract(ray1.dir, -m_eyeball->intNormalInterpolated(), m_eyeball->refractiveIndex));
	const Ray ray3(intpoint, dir);

	if (std::isnan(ray3.dir.x))
	{
        *freePosition = currentPosition;
		return Ray(NaN);
	}

    localPaths[currentPosition++] = Line(ray2.origin, ray3.origin);

    *freePosition = currentPosition;
	return ray3;
}