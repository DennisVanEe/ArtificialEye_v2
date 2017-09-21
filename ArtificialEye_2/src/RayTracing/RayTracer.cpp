#include "RayTracer.hpp"

#include "../Rendering/RenderingUtilities.hpp"

#undef min
#undef max

ee::RayTracer& ee::RayTracer::initialize(std::vector<glm::vec3> positions, UVMeshSphere sphere, RayTracerParam param)
{
    static RayTracer rayTracer(positions, sphere, param);
    return rayTracer;
}

void ee::RayTracer::raytrace()
{
    const float radialInc = 1.f / m_heightResolution;
    const float circInc = (2 * radialInc * glm::pi<float>()) / m_widthResolution; // this is the base that defines difference between points
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

ee::RayTracer::RayTracer(std::vector<glm::vec3> positions, UVMeshSphere sphere, RayTracerParam param) :
    m_ETA(param.m_enviRefractiveIndex / param.m_lensRefractiveIndex),
    m_invETA(param.m_lensRefractiveIndex / param.m_enviRefractiveIndex),
    m_widthResolution(param.m_widthResolution),
    m_heightResolution(param.m_heightResolution),
    m_lens(sphere),
    m_rayOrigins(positions)    
{    
    m_resultColors.resize(m_rayOrigins.size());

    assert(m_heightResolution > 0);
    assert(m_widthResolution > 0);

    if (sphere.getNumLongitudes() & 1 == 1)
    {
        throw std::runtime_error("Lens UVSphere's number of longitudes must be even for the ray tracer.");
    }

    // Calcualte the positions where the rays will intersect
    // the lens:
    const float rInc = 1.f / m_heightResolution;
    const float cInc = (2 * rInc * PI) / m_widthResolution;

    for (float r = rInc; r < 1.f; r += rInc)
    {
        const float cIncAngle = PI2 * (cInc / (2 * r * PI));
        for (float c = 0; c < PI2; c += cIncAngle)
        {
            const glm::vec3 dest(r * cosf(c), r * sinf(c), 0.f);
            m_cachedPoints.push_back(dest);
        }
    }

    // Set up the drawalb rays:
    ee::Renderer::addTexturePack("lineTextPack", ee::LineUniColorTextPack(param.m_rayColor));
    for (std::size_t i = 0; i < m_rayOrigins.size() * m_cachedPoints.size(); i++)
    {
        m_drawableLines.push_back(DrawLensRayPath("lineTextPack"));
    }

    testNormals[0] = new DrawLine("lineTextPack", glm::vec3(), glm::vec3());
    testNormals[1] = new DrawLine("lineTextPack", glm::vec3(), glm::vec3());
    testNormals[2] = new DrawLine("lineTextPack", glm::vec3(), glm::vec3());

    ee::Renderer::addDrawable(testNormals[0]);
    ee::Renderer::addDrawable(testNormals[1]);
    ee::Renderer::addDrawable(testNormals[2]);

    for (DrawLensRayPath& rayPack : m_drawableLines)
    {
        rayPack.sendToDrawable();
    }

    // Allocate space for the spline caches:
    m_latSplines.resize(m_lens.getNumLatitudes()); // TODO: decrease the size in this case because it really isn't the entire thing
    m_lonSplinesTop.resize(m_lens.getNumLongitudes());
    m_lonSplinesBottom.resize(m_lens.getNumLongitudes());
}

bool ee::RayTracer::lensRefract(const Ray startRay, LensRayPath* o_rayPath)
{
    LensRayPath result;

    Mesh* const lensMesh = m_lens.getMesh();
    const Ray entryRay(startRay.m_origin, glm::normalize(startRay.m_dir));

    const std::pair<std::size_t, glm::vec3> entryIntersection = nearestIntersectionMesh(lensMesh, entryRay);
    // assert(entryIntersection.first < lensMesh->getNumMeshFaces());
    if (entryIntersection.first >= lensMesh->getNumMeshFaces())
    {
        return false;
    }

    // get point of intersection:
    const MeshFace& entryFace = lensMesh->getMeshFace(entryIntersection.first);

    result.m_entry = Line(entryRay.m_origin, entryIntersection.second);

    const glm::vec3 entryNormal = getNormal(entryIntersection.first, entryIntersection.second); // glm::vec3(lensMesh->getNormalModelTrans() * glm::vec4(getNormal(entryIntersection.first, entryIntersection.second), 0.f));
    const glm::vec3 entryRefraction = glm::normalize(cust::refract(entryRay.m_dir, entryNormal, m_ETA));

    // now let's find the next intersection:
    const std::pair<std::size_t, glm::vec3> passIntersection = nearestIntersectionMesh(lensMesh, Ray(entryIntersection.second, entryRefraction), entryIntersection.first);
    if (passIntersection.first >= lensMesh->getNumMeshFaces()) { return false; }

    // assign the line:
    result.m_pass = Line(entryIntersection.second, passIntersection.second);

    // now calulcate the next part:
    const MeshFace& passFace = lensMesh->getMeshFace(passIntersection.first);
    const glm::vec3 passNormal = -getNormal(passIntersection.first, passIntersection.second); //glm::vec3(lensMesh->getNormalModelTrans() * glm::vec4(getNormal(passIntersection.first, passIntersection.second), 0.f));
    const glm::vec3 passRefraction = glm::normalize(cust::refract(entryRefraction, passNormal, m_invETA));
    // TODO: fix the weirdness that is happening

    // now set the last part:
    result.m_end = Ray(passIntersection.second, passRefraction);

    *o_rayPath = result;
    return result.m_end.m_dir != glm::vec3();
}

glm::vec3 ee::RayTracer::getNormal(std::size_t triangle, glm::vec3 interPoint)
{
    Mesh* const lensMesh = m_lens.getMesh();
    const MeshFace& face = lensMesh->getMeshFace(triangle);

    glm::vec3 trianglePoints[3]; // get all of the points of the triangle: // don't bother with this
    for (int i = 0; i < 3; i++) { trianglePoints[i] = transPoint3(lensMesh->getModelTrans(), lensMesh->getVertex(face.m_indices[i]).m_position); }

     glm::vec3 _p[3]; // get all of the points of the triangle: // don't bother with this
    for (int i = 0; i < 3; i++) { _p[i] = lensMesh->getVertex(face.m_indices[i]).m_position; }

    glm::vec4 t = lensMesh->getModelTrans() * glm::vec4(lensMesh->getVertex(face.m_indices[0]).m_position, 1.f);

    bool isCap = false; // check whether or not it is a cap
    for (int i = 0; i < 3; i++) { isCap = (face.m_indices[i] == 0 || face.m_indices[i] == (lensMesh->getNumVertices() - 1)) || isCap; }

    if (isCap)
    {
        return lensMesh->getNormal(triangle); // for now
    }
    else
    {
        unsigned latID[3]; // stores the latitude and longitude spline ids:
        unsigned lonID[3];
        for (unsigned i = 0; i < 3; i++)
        {
            latID[i] = m_lens.getLatitudeIndex(face.m_indices[i]);
            lonID[i] = m_lens.getLongitudeIndex(face.m_indices[i]);

            if (latID[i] > m_lens.getMuscleBegin() && latID[i] < m_lens.getMuscleEnd()) // in muslce
            {
                return glm::vec3(); // 0 vector normal, no can do if this occurs
            }
        }

        // Get the pairs of points that share a common spline:
        unsigned latTriPointID_pair[2];
        unsigned lonTriPointID_pair[2];
        for (unsigned i = 0; i < 3; i++)
        {
            for (unsigned j = 0; j < 3; j++)
            {
                if (i != j)
                {
                    if (latID[i] == latID[j])
                    {
                        latTriPointID_pair[0] = i;
                        latTriPointID_pair[1] = j;
                    }

                    if (lonID[i] == lonID[j])
                    {
                        lonTriPointID_pair[0] = i;
                        lonTriPointID_pair[1] = j;
                    }
                }
            }
        }

        unsigned latTriPointID_single = 4;
        unsigned lonTriPointID_single = 4;
        for (int i = 0; i < 3; i++)
        {
            if (latID[i] != latID[latTriPointID_pair[0]] && latID[i] != latID[latTriPointID_pair[1]])
            {
                latTriPointID_single = i;
            }

            if (lonID[i] != lonID[lonTriPointID_pair[0]] && lonID[i] != lonID[lonTriPointID_pair[1]])
            {
                lonTriPointID_single = i;
            }
        }
        assert(latTriPointID_single < 4);
        assert(lonTriPointID_single < 4);

        // Build the splines:
        bool bottom = latID[0] < m_lens.getMuscleBegin();
        buildLatSpline(latID[latTriPointID_pair[0]]);
        buildLatSpline(latID[latTriPointID_single]);
        buildLonSpline(lonID[lonTriPointID_pair[0]], bottom);
        buildLonSpline(lonID[lonTriPointID_single], bottom);

        // Now get the splines for testing:
        // The starting point shall be the one that is shared:
        // I'M TIRED RIGHT NOW
        unsigned triPointID_start;
        unsigned triPointIDLat_end;
        unsigned triPointIDLon_end;
        bool done = false;
        for (int i = 0; i < 2 && !done; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (latTriPointID_pair[i] == lonTriPointID_pair[j])
                {
                    triPointID_start = latTriPointID_pair[i];
                    triPointIDLat_end = latTriPointID_pair[i == 0 ? 1 : 0];
                    triPointIDLon_end = lonTriPointID_pair[j == 0 ? 1 : 0];
                    done = true;
                }
            }
        }

        const glm::vec3 latVector = transVector3(lensMesh->getModelTrans(),
            lensMesh->getVertex(face.m_indices[triPointIDLat_end]).m_position - lensMesh->getVertex(face.m_indices[triPointID_start]).m_position);
        const glm::vec3 lonVector = transVector3(lensMesh->getModelTrans(),
            lensMesh->getVertex(face.m_indices[triPointIDLon_end]).m_position - lensMesh->getVertex(face.m_indices[triPointID_start]).m_position);

        const glm::vec3 latToIntVector = interPoint - transPoint3(lensMesh->getModelTrans(), lensMesh->getVertex(face.m_indices[triPointID_start]).m_position);
        const glm::vec3 lonToIntVector = interPoint - transPoint3(lensMesh->getModelTrans(), lensMesh->getVertex(face.m_indices[triPointID_start]).m_position);

                                                             // now project it:
        const float latRatio = glm::dot(latToIntVector, glm::normalize(latVector)) / glm::length(latVector);
        const float lonRatio = glm::dot(lonToIntVector, glm::normalize(lonVector)) / glm::length(lonVector);
        
        const double latParam0 = latSplineParameter(latID[latTriPointID_pair[0]], lonID[triPointID_start], lonID[triPointIDLat_end], latRatio);
        const double latParam1 = latSplineParameter(latID[latTriPointID_single], lonID[triPointID_start], lonID[triPointIDLat_end], latRatio);
        const double lonParam0 = lonSplineParameter(lonID[lonTriPointID_pair[0]], latID[triPointID_start], latID[triPointIDLon_end], lonRatio, bottom);
        const double lonParam1 = lonSplineParameter(lonID[lonTriPointID_single], latID[triPointID_start], latID[triPointIDLon_end], lonRatio, bottom);

        const glm::vec3 p0 = getLatParamValue(latID[latTriPointID_pair[0]], latParam0);
        const glm::vec3 p1 = getLatParamValue(latID[latTriPointID_single], latParam1);
        const glm::vec3 p2 = getLonParamValue(lonID[lonTriPointID_pair[0]], lonParam0, bottom);
        const glm::vec3 p3 = getLonParamValue(lonID[lonTriPointID_single], lonParam1, bottom);
        
        const glm::vec3 latTangent = getLatTangent(latID[triPointID_start], latID[latTriPointID_single], latParam0, latParam1, lonRatio);
        const glm::vec3 lonTangent = getLonTangent(lonID[triPointID_start], lonID[lonTriPointID_single], lonParam0, lonParam1, latRatio, bottom);

        const glm::vec3 thePoint0 = (latTangent * lonRatio) + getLatParamValue(latID[latTriPointID_single], latParam1);
        const glm::vec3 thePoint1 = (lonTangent * latRatio) + getLonParamValue(lonID[lonTriPointID_single], lonParam1, bottom);

        const glm::vec3 splineNormal = glm::normalize(glm::cross(latTangent, lonTangent));
        const glm::vec3 faceNormal = lensMesh->getNormal(triangle);
        const glm::vec3 result = glm::dot(splineNormal, faceNormal) >= 0 ? splineNormal : -splineNormal;

        if (set)
        {
             //testNormals[0]->setRay(Ray(interPoint, latTangent), 1.f);
             //testNormals[1]->setRay(Ray(interPoint, faceNormal), 1.f);
             // testNormals[2]->setRay(Ray(interPoint, result), 1.f);
            // set = false;
        }

        return result; // glm::dot(splineNormal, faceNormal) >= 0 ? splineNormal : -splineNormal;
    }
}

void ee::RayTracer::buildLatSpline(unsigned latID)
{
    if (m_latSplines[latID].m_valid)
    {
        return;
    }

    const auto& latitudes = m_lens.getLatitudes(latID);
    std::vector<glm::dvec3> points; // will store the points for the spline to use:
    for (const std::size_t vertexID : latitudes)
    {
        const glm::vec3 p = transPoint3(m_lens.getMesh()->getModelTrans(), m_lens.getMesh()->getVertex(vertexID).m_position);
        points.push_back(glm::dvec3(p));
    }
    const glm::vec3 p = transPoint3(m_lens.getMesh()->getModelTrans(), m_lens.getMesh()->getVertex(latitudes[0]).m_position);
    points.push_back(glm::dvec3(p));

    alglib::real_2d_array data;
    data.setcontent(points.size(), 3, reinterpret_cast<const double*>(points.data()));
    alglib::pspline3build(data, points.size(), 2, 0, m_latSplines[latID].m_spline);
    m_latSplines[latID].m_valid = true;
}

void ee::RayTracer::buildLonSpline(const unsigned lonID, const bool bottom)
{
    std::vector<SplineCache>& lonSplines = bottom ? m_lonSplinesBottom : m_lonSplinesTop;

    Mesh* const lensMesh = m_lens.getMesh();
    
    const unsigned halfNumLon = m_lens.getNumLongitudes() / 2;
    const unsigned lonID0 = lonID < halfNumLon ? lonID : lonID - halfNumLon;
    const unsigned lonID1 = lonID < halfNumLon ? lonID + halfNumLon : lonID;

    // assert(lonID0 != 0);

    if (lonSplines[lonID0].m_valid)
    {
        return;
    }

    auto& longitudes0 = m_lens.getLongitudes(lonID0);
    auto& longitudes1 = m_lens.getLongitudes(lonID1);

    const unsigned startPoint0 = bottom ? m_lens.getMuscleBegin() : m_lens.getMuscleEnd();
    const unsigned endPoint0   = bottom ? 0 : longitudes0.size() - 1;
    const unsigned startPoint1 = bottom ? 0 : longitudes1.size() - 1;
    const unsigned endPoint1   = startPoint0;

    const unsigned endVertexID = bottom ? 0 : lensMesh->getNumVertices() - 1;
    const int      dec = bottom ? -1 : 1;

    // TODO: fix the spline creation occuring in this part of the code.

    std::vector<glm::vec3> temp;
    for (std::size_t p : longitudes0)
    {
        temp.push_back(lensMesh->getVertex(p).m_position);
    }

    std::vector<glm::dvec3> points;
    int vertexIDID = startPoint0 - dec; // this is to offset the value so we can reach the correct point
    do
    {
        vertexIDID = vertexIDID + dec;
        const glm::vec3 p = transPoint3(lensMesh->getModelTrans(), lensMesh->getVertex(longitudes0[vertexIDID]).m_position);
        points.push_back(glm::dvec3(p));
    } while (vertexIDID != endPoint0);
    points.push_back(glm::dvec3(transPoint3(lensMesh->getModelTrans(), lensMesh->getVertex(endVertexID).m_position)));
    vertexIDID = startPoint1 + dec; // same thing here
    do
    {
        vertexIDID = vertexIDID - dec;
        const glm::vec3 p = transPoint3(lensMesh->getModelTrans(), lensMesh->getVertex(longitudes1[vertexIDID]).m_position);
        points.push_back(glm::dvec3(p));
    } while (vertexIDID != endPoint1);

    alglib::real_2d_array data;
    data.setcontent(points.size(), 3, reinterpret_cast<const double*>(points.data()));
    alglib::pspline3build(data, points.size(), 2, 0, lonSplines[lonID0].m_spline);
    lonSplines[lonID0].m_valid = true;
}

double ee::RayTracer::latSplineParameter(unsigned splineID, unsigned startLonID, unsigned endLonID, float parameter)
{
    alglib::real_1d_array data;
    alglib::ae_int_t size;
    alglib::pspline3parametervalues(m_latSplines[splineID].m_spline, size, data);

    const double startParam = data[startLonID];
    const double endParam = data[endLonID];

    const double relativeParam = (endParam - startParam) * parameter;
    return relativeParam + startParam;
}

double ee::RayTracer::lonSplineParameter(unsigned splineID, unsigned startLatID, unsigned endLatID, float parameter, bool bottom)
{
    const unsigned halfNumLon = m_lens.getNumLongitudes() / 2;
    const unsigned splineID0 = splineID < halfNumLon ? splineID : splineID - halfNumLon;
    const unsigned splineID1 = splineID < halfNumLon ? splineID + halfNumLon : splineID;

    alglib::real_1d_array data;
    alglib::ae_int_t size;
    if (bottom)
    {
        assert(m_lonSplinesBottom[splineID0].m_valid);
        alglib::pspline3parametervalues(m_lonSplinesBottom[splineID0].m_spline, size, data);
    }
    else
    {
        assert(m_lonSplinesTop[splineID0].m_valid);
        alglib::pspline3parametervalues(m_lonSplinesTop[splineID0].m_spline, size, data);
    }

    unsigned newStartLonID, newEndLonID;
    if (splineID < halfNumLon) // it is on the other side of the lateral side
    {
        newStartLonID = bottom ? m_lens.getMuscleBegin() - startLatID : startLatID - m_lens.getMuscleEnd();
        newEndLonID = bottom ? m_lens.getMuscleBegin() - endLatID : endLatID - m_lens.getMuscleEnd();
    }
    else
    {
        newStartLonID = bottom ? m_lens.getMuscleBegin() + 2 + startLatID : ((m_lens.getNumLatitudes() - 1) - m_lens.getMuscleEnd()) + 2 + ((m_lens.getNumLatitudes() - 1) - startLatID);
        newEndLonID = bottom ? m_lens.getMuscleBegin() + 2 + endLatID : ((m_lens.getNumLatitudes() - 1) - m_lens.getMuscleEnd()) + 2 + ((m_lens.getNumLatitudes() - 1) - endLatID);
    }

    const double startParam = data[newStartLonID];
    const double endParam = data[newEndLonID];

    const double relativeParam = (endParam - startParam) * parameter;
    return relativeParam + startParam;
}

glm::vec3 ee::RayTracer::getLatParamValue(unsigned splineID, double param)
{
    param = param < 0 ? 0 : param;
    double x, y, z;
    alglib::pspline3calc(m_latSplines[splineID].m_spline, param, x, y, z);
    return glm::vec3(x, y, z);
}

glm::vec3 ee::RayTracer::getLonParamValue(unsigned splineID, double param, bool bottom)
{
    param = param < 0 ? 0 : param;

    const unsigned halfNumLon = m_lens.getNumLongitudes() / 2;
    const unsigned splineID0 = splineID < halfNumLon ? splineID : splineID - halfNumLon;

    double x, y, z;
    if (bottom)
    {
        alglib::pspline3calc(m_lonSplinesBottom[splineID0].m_spline, param, x, y, z);
    }
    else
    {
        alglib::pspline3calc(m_lonSplinesTop[splineID0].m_spline, param, x, y, z);

    }
    return glm::vec3(x, y, z);
}

glm::vec3 ee::RayTracer::getLatParamTangent(unsigned splineID, double param)
{
    param = param < 0 ? 0 : param;
    double x, y, z;
    alglib::pspline3tangent(m_latSplines[splineID].m_spline, param, x, y, z);
    return glm::vec3(x, y, z);
}

glm::vec3 ee::RayTracer::getLonParamTangent(unsigned splineID, double param, bool bottom)
{
    param = param < 0 ? 0 : param;

    const unsigned halfNumLon = m_lens.getNumLongitudes() / 2;
    const unsigned splineID0 = splineID < halfNumLon ? splineID : splineID - halfNumLon;

    double x, y, z;
    if (bottom)
    {
        alglib::pspline3tangent(m_lonSplinesBottom[splineID0].m_spline, param, x, y, z);
    }
    else
    {
        alglib::pspline3tangent(m_lonSplinesTop[splineID0].m_spline, param, x, y, z);

    }
    return glm::vec3(x, y, z);
}

glm::vec3 ee::RayTracer::getLatTangent(unsigned startSplineID, unsigned endSplineID, double startParam, double endParam, float lonRatio)
{
    assert(startSplineID != endSplineID);

    const glm::vec3 startBeginPoint = getLatParamValue(startSplineID, startParam);
    const glm::vec3 endBeginPoint = getLatParamValue(endSplineID, endParam);
    const glm::vec3 beginVector = endBeginPoint - startBeginPoint;

    const glm::vec3 startTangent = getLatParamTangent(startSplineID, startParam); assert(startTangent != glm::vec3());
    const glm::vec3 endTangent = getLatParamTangent(endSplineID, endParam); assert(endTangent != glm::vec3());

    const glm::vec3 startEndPoint = startBeginPoint + startTangent;
    const glm::vec3 endEndPoint = endBeginPoint + endTangent;
    const glm::vec3 endVector = endEndPoint - startEndPoint;

    const glm::vec3 tangentPoint0 = glm::normalize(beginVector) * (lonRatio * glm::length(beginVector)) + startBeginPoint;
    const glm::vec3 tangentPoint1 = glm::normalize(endVector) * (lonRatio * glm::length(endVector)) + startEndPoint;

    return glm::normalize(tangentPoint1 - tangentPoint0);
}

glm::vec3 ee::RayTracer::getLonTangent(unsigned startSplineID, unsigned endSplineID, double startParam, double endParam, float latRatio, bool bottom)
{
    assert(startSplineID != endSplineID);

    const glm::vec3 startBeginPoint = getLonParamValue(startSplineID, startParam, bottom);
    const glm::vec3 endBeginPoint = getLonParamValue(endSplineID, endParam, bottom);

    const glm::vec3 startTangent = getLonParamTangent(startSplineID, startParam, bottom); assert(startTangent != glm::vec3());
    const glm::vec3 endTangent = getLonParamTangent(endSplineID, endParam, bottom); assert(endTangent != glm::vec3());

    const glm::vec3 startEndPoint = startBeginPoint + startTangent;
    const glm::vec3 endEndPoint = endBeginPoint + endTangent;

    const glm::vec3 beginVector = endBeginPoint - startBeginPoint;
    const glm::vec3 endVector = endEndPoint - startEndPoint;

    const glm::vec3 tangentPoint0 = glm::normalize(beginVector) * (latRatio * glm::length(beginVector)) + startBeginPoint;
    const glm::vec3 tangentPoint1 = glm::normalize(endVector) * (latRatio * glm::length(endVector)) + startEndPoint;

    return glm::normalize(tangentPoint1 - tangentPoint0);
}

glm::vec3 ee::RayTracer::raytrace(std::size_t pos, const float rInc, const float cInc)
{
    set = true;

    m_latSplines.clear(); m_latSplines.resize(m_lens.getNumLatitudes());
    m_lonSplinesBottom.clear(); m_lonSplinesBottom.resize(m_lens.getNumLongitudes());
    m_lonSplinesTop.clear(); m_lonSplinesTop.resize(m_lens.getNumLongitudes());

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