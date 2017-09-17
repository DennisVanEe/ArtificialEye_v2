#include "RayTracer.hpp"

ee::RayTracer& ee::RayTracer::initialize(std::vector<glm::vec3> positions, const Mesh* lens, float lensRefractiveIndex, RayTracerParam param, glm::vec3 rayColor)
{
    static RayTracer rayTracer(positions, lens, lensRefractiveIndex, param, rayColor);
    return rayTracer;
}

void ee::RayTracer::raytrace()
{
    const float radialInc = 1.f / m_parameters.m_heightResolution;
    const float circInc = (2 * radialInc * glm::pi<float>()) / m_parameters.m_widthResolution; // this is the base that defines difference between points
                                                                                               // this is to maintain equal distribution
    // this can easily be parallized if it becomes too hard:
    for (std::size_t i = 0; i < m_rayOrigins.size(); i++) // because there will be 3 different rays
    {
        m_resultColors[i] = raytrace(i, radialInc, circInc);
    }
}

const std::vector<glm::vec3>& ee::RayTracer::getResultColors() const
{
    return m_resultColors;
}

ee::RayTracer::RayTracer(std::vector<glm::vec3> positions, const Mesh* lens, float lensRefractiveIndex, RayTracerParam param, glm::vec3 rayColor) :
    m_lens(lens),
    m_ETA(1.f / lensRefractiveIndex),
    m_parameters(param),
    m_rayOrigins(positions)
{
    m_resultColors.resize(m_rayOrigins.size());

    assert(m_parameters.m_heightResolution > 0);
    assert(m_parameters.m_widthResolution > 0);

    // calculate all of the necessary values:
    const float rInc = 1.f / m_parameters.m_heightResolution;
    const float cInc = (2 * rInc * PI) / m_parameters.m_widthResolution;

    for (float r = rInc; r < 1.f; r += rInc)
    {
        const float cIncAngle = PI2 * (cInc / (2 * r * PI));
        for (float c = 0; c < PI2; c += cIncAngle)
        {
            const glm::vec3 dest(r * cosf(c), r * sinf(c), 0.f);
            // assert(dest != glm::vec3(1.f, 0.f, 0.f));
            m_cachedPoints.push_back(dest);
        }
    }

    // set up all the drawables:
    ee::Renderer::addTexturePack("lineTextPack", ee::LineUniColorTextPack(rayColor));
    for (std::size_t i = 0; i < m_rayOrigins.size() * m_cachedPoints.size(); i++)
    {
        m_drawableLines.push_back(DrawLensRayPath("lineTextPack"));
    }

    for (DrawLensRayPath& rayPack : m_drawableLines)
    {
        rayPack.sendToDrawable();
    }

    // assert(m_drawableLines.size() == m_rayOrigins.size() * m_cachedPoints.size());
}

bool ee::RayTracer::lensRefract(const Ray startRay, LensRayPath* o_rayPath)
{
    LensRayPath result;

    const Ray entryRay(startRay.m_origin, glm::normalize(startRay.m_dir));

    const std::pair<std::size_t, glm::vec3> entryIntersection = nearestIntersection(m_lens, entryRay);
    assert(entryIntersection.first < m_lens->getNumMeshFaces());

    result.m_entry = Line(entryRay.m_origin, entryIntersection.second);

    const MeshFace& entryFace = m_lens->getMeshFace(entryIntersection.first);
    const glm::vec3 entryNormal = glm::vec3(m_lens->getNormalModelTrans() * glm::vec4(m_lens->getNormal(entryIntersection.first), 0.f));
    const glm::vec3 entryRefraction = glm::normalize(cust::refract(entryRay.m_dir, entryNormal, m_ETA));

    // now let's find the next intersection:
    const std::pair<std::size_t, glm::vec3> passIntersection = nearestIntersection(m_lens, Ray(entryIntersection.second, entryRefraction), entryIntersection.first);
    assert(passIntersection.first < m_lens->getNumMeshFaces());

    // assign the line:
    result.m_pass = Line(entryIntersection.second, passIntersection.second);

    // now calulcate the next part:
    const MeshFace& passFace = m_lens->getMeshFace(passIntersection.first);
    const glm::vec3 passNormal = -glm::vec3(m_lens->getNormalModelTrans() * glm::vec4(m_lens->getNormal(passIntersection.first), 0.f));
    const glm::vec3 passRefraction = glm::normalize(cust::refract(entryRefraction, entryNormal, 1.f / m_ETA));

    // now set the last part:
    result.m_end = Ray(passIntersection.second, passRefraction);

    *o_rayPath = result;
    return result.m_end.m_dir != glm::vec3();
}

glm::vec3 ee::RayTracer::raytrace(std::size_t pos, const float rInc, const float cInc)
{
    LensRayPath path;
    for (std::size_t i = 0; i < m_rayOrigins.size(); i++)
    {
        for (std::size_t j = 0; j < m_cachedPoints.size(); j++)
        {
            const Ray currRay(glm::vec3(m_cachedPoints[j].x, m_cachedPoints[j].y, m_rayOrigins[i].z), glm::vec3(0.f, 0.f, 1.f)); // m_cachedPoints[j] - m_rayOrigins[i]);
            if (lensRefract(currRay, &path))
            {
                std::size_t index = i * m_cachedPoints.size() + j;
                m_drawableLines[index].m_begin.setLine(path.m_entry);
                m_drawableLines[index].m_middle.setLine(path.m_pass);
                m_drawableLines[index].m_end.setRay(path.m_end, 10.f);
            }
        }
    }

    return glm::vec3(); // for now
}

void ee::RayTracer::DrawLensRayPath::sendToDrawable()
{
    ee::Renderer::addDrawable(&m_begin);
    ee::Renderer::addDrawable(&m_middle);
    ee::Renderer::addDrawable(&m_end);
}

ee::RayTracer::DrawLensRayPath::DrawLensRayPath(std::string textPack) :
    m_begin(textPack, glm::vec3(), glm::vec3()),
    m_middle(textPack, glm::vec3(), glm::vec3()),
    m_end(textPack, glm::vec3(), glm::vec3()) 
{
}