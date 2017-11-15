#include "RayTracer.hpp"

#undef min
#undef max

ee::RayTracer& ee::RayTracer::initialize(std::vector<Vec3> positions, Lens sphere, RayTracerParam param)
{
    static RayTracer rayTracer(positions, sphere, param);
    return rayTracer;
}

void ee::RayTracer::raytrace()
{
    const Float radialInc = 1.0 / m_parameters.m_heightResolution;
    const Float circInc = (PI2 * radialInc) / m_parameters.m_widthResolution; // this is the base that defines difference between points
                                                                                               // this is to maintain equal distribution
    // this can easily be parallized if it becomes too hard:
    for (int i = 0; i < m_rayOrigins.size(); i++) // because there will be 3 different rays
    {
        m_resultColors[i] = raytrace(i, radialInc, circInc);
    }
}

const std::vector<ee::Vec3>& ee::RayTracer::getResultColors() const
{
    return m_resultColors;
}

ee::RayTracer::RayTracer(std::vector<Vec3> positions, Lens sphere, RayTracerParam param) :
    m_parameters(param),
    m_lens(sphere),
    m_rayOrigins(positions)    
{    
    m_resultColors.resize(m_rayOrigins.size());

    assert(m_parameters.m_heightResolution > 0);
    assert(m_parameters.m_widthResolution > 0);

    if ((sphere.getNumLongitudes() & 1) == 1)
    {
        throw std::runtime_error("Lens UVSphere's number of longitudes must be even for the ray tracer.");
    }

    // Calcualte the positions where the rays will intersect
    // the lens:
    const Float rInc = 1.0 / m_parameters.m_heightResolution;
    const Float cInc = (2 * rInc * PI) / m_parameters.m_widthResolution;

    for (Float r = rInc; r < 1.0; r += rInc)
    {
        const Float cIncAngle = PI2 * (cInc / (PI2 * r));
        for (Float c = 0; c < PI2; c += cIncAngle)
        {
            const Vec3 dest(r * std::cos(c), r * std::sin(c), 0.0);
            m_cachedPoints.push_back(dest);
        }
    }

    // Set up the drawable rays:
    ee::Renderer::addTexturePack("lineTextPack", ee::LineUniColorTextPack(param.m_rayColor));
    //ee::Renderer::addTexturePack("lineTextPack2", ee::LineUniColorTextPack(Vec3(0.0, 1.0, 0.0)));
    //ee::Renderer::addTexturePack("lineTextPack3", ee::LineUniColorTextPack(Vec3(0.0, 0.0, 1.0)));
    for (int i = 0; i < m_rayOrigins.size() * m_cachedPoints.size(); i++)
    {
        m_drawableLines.push_back(DrawLensRayPath("lineTextPack"));
    }

    //for (int i = 0; i < 2 * m_cachedPoints.size(); i++)
    //{
    //    testNormals.push_back(new DrawLine("lineTextPack3", glm::vec3(), glm::vec3()));
    //}

    //for (int i = 0; i < testNormals.size(); i++)
    //{
    //    ee::Renderer::addDrawable(testNormals[i]);
    //}

    for (DrawLensRayPath& rayPack : m_drawableLines)
    {
        rayPack.sendToDrawable();
    }
}

bool ee::RayTracer::lensRefract(const Ray startRay, LensRayPath* o_rayPath, unsigned id)
{
    LensRayPath result;

    // intersect the ray with the cornea initially:
    const Ray airToCornea(startRay.m_origin, glm::normalize(startRay.m_dir));
    auto corneaInternsection = intersectCorneaSphere(airToCornea, 1.0);
    const Vec3 corneaIntersection = (corneaInternsection.first * airToCornea.m_dir) + airToCornea.m_origin;
    result.m_airToCornea = Line(airToCornea.m_origin, corneaIntersection);

    const Vec3 airToCorneaRefraction = glm::normalize(cust::refract(airToCornea.m_dir, corneaInternsection.second, 
        m_parameters.m_enviRefractiveIndex / m_parameters.m_eyeballRefractiveIndex));

    // now intersect this with the lens itself
    const Mesh* const lensMesh = m_lens.getMesh();
    const Ray corneaToLens(corneaIntersection, airToCorneaRefraction);

    const std::pair<std::size_t, Vec3> entryLensIntersection = nearestIntersectionMesh(lensMesh, corneaToLens);
    // assert(entryIntersection.first < lensMesh->getNumMeshFaces());
    if (entryLensIntersection.first >= lensMesh->getNumMeshFaces())
    {
        return false;
    }

    // get point of intersection:
    const MeshFace& entryFace = lensMesh->getMeshFace(entryLensIntersection.first);

    result.m_corneaToLens = Line(corneaToLens.m_origin, entryLensIntersection.second);

    const Vec3 entryLensNormal = getNormal(entryLensIntersection.first, entryLensIntersection.second, id);
    
    Float radiusOfIntersection = glm::length(Vec2(entryLensIntersection.second.x, entryLensIntersection.second.y));
    Float actualLensRefrective = m_parameters.m_lensRefractiveIndex_end * (radiusOfIntersection)+m_parameters.m_lensRefractiveIndex_middle * (1.0 - radiusOfIntersection);

    const Vec3 entryLensRefraction = glm::normalize(cust::refract(corneaToLens.m_dir, entryLensNormal,
        m_parameters.m_eyeballRefractiveIndex / actualLensRefrective));

    // now let's find the next intersection:
    const std::pair<std::size_t, Vec3> passIntersection = nearestIntersectionMesh(lensMesh, 
        Ray(entryLensIntersection.second, entryLensRefraction), entryLensIntersection.first);
    if (passIntersection.first >= lensMesh->getNumMeshFaces()) { return false; }

    // assign the line:
    result.m_inLens = Line(entryLensIntersection.second, passIntersection.second);



    // now calulcate the next part:
    const MeshFace& passFace = lensMesh->getMeshFace(passIntersection.first);
    const Vec3 passNormal = -getNormal(passIntersection.first, passIntersection.second, UINT_MAX);
    
    const Vec3 passRefraction = glm::normalize(cust::refract(entryRefraction, passNormal, 1.f));

    // now set the last part:
    result.m_end = Ray(passIntersection.second, passRefraction);

    *o_rayPath = result;
    return result.m_end.m_dir != Vec3();
}

ee::Vec3 ee::RayTracer::getNormal(int triangle, Vec3 interPoint, unsigned id)
{
    Mesh* const lensMesh = m_lens.getMesh();
    MeshFace face = lensMesh->getMeshFace(triangle);
    Vertex vert0 = lensMesh->getTransformedVertex(face(0));
    Vertex vert1 = lensMesh->getTransformedVertex(face(1));
    Vertex vert2 = lensMesh->getTransformedVertex(face(2));

    Float u, v, w;
    baryCentric(interPoint, vert0.m_position, vert1.m_position, vert2.m_position, u, v, w);
    Vec3 normal = glm::normalize(vert0.m_normal * u + vert1.m_normal * v + vert2.m_normal * w);
    if (id != UINT_MAX)
    {
        //testNormals[id]->setRay(Ray(interPoint, normal), 10.0);
    }
    return normal;
}

ee::Vec3 ee::RayTracer::raytrace(int pos, const Float rInc, const Float cInc)
{
    for (std::size_t i = 0; i < m_rayOrigins.size(); i++)
    {
        for (std::size_t j = 0; j < m_cachedPoints.size(); j++)
        {
            const Ray currRay(Vec3(m_cachedPoints[j].x, m_cachedPoints[j].y, m_rayOrigins[i].z), Vec3(0.0, 0.0, 1.0)); // m_cachedPoints[j] - m_rayOrigins[i]);
            LensRayPath path;
            if (lensRefract(currRay, &path, j))
            {
                std::size_t index = i * m_cachedPoints.size() + j;
                m_drawableLines[index].m_begin.setLine(path.m_entry);
                m_drawableLines[index].m_middle.setLine(path.m_pass);
                m_drawableLines[index].m_end.setRay(path.m_end, 10.f);
            }
        }
    }

    return Vec3(); // for now
}