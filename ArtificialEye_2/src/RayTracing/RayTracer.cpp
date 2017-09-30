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
            const glm::vec3 dest(r * std::cos(c), r * std::sin(c), 0.f);
            m_cachedPoints.push_back(dest);
        }
    }

    // Set up the drawalb rays:
    ee::Renderer::addTexturePack("lineTextPack", ee::LineUniColorTextPack(param.m_rayColor));
    ee::Renderer::addTexturePack("lineTextPack2", ee::LineUniColorTextPack(glm::vec3(0.f, 1.f, 0.f)));
    ee::Renderer::addTexturePack("lineTextPack3", ee::LineUniColorTextPack(glm::vec3(0.f, 0.f, 1.f)));
    for (std::size_t i = 0; i < m_rayOrigins.size() * m_cachedPoints.size(); i++)
    {
        m_drawableLines.push_back(DrawLensRayPath("lineTextPack"));
    }

    for (int i = 0; i < sizeof(testNormals) / sizeof(void*); i++)
    {
        if (i % 3 == 0)
        {
            testNormals[i] = new DrawLine("lineTextPack3", glm::vec3(), glm::vec3());
        }
        else
        {
            testNormals[i] = new DrawLine("lineTextPack2", glm::vec3(), glm::vec3());
        }
    }
    for (int i = 0; i < sizeof(testNormals) / sizeof(void*); i++)
        ee::Renderer::addDrawable(testNormals[i]);

    for (DrawLensRayPath& rayPack : m_drawableLines)
    {
        rayPack.sendToDrawable();
    }

    // Allocate space for the spline caches:
    m_latSplines.resize(m_lens.getNumLatitudes()); // TODO: decrease the size in this case because it really isn't the entire thing
    m_lonSplinesTop.resize(m_lens.getNumLongitudes());
    m_lonSplinesBottom.resize(m_lens.getNumLongitudes());
}

bool ee::RayTracer::lensRefract(const Ray startRay, LensRayPath* o_rayPath, unsigned id)
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

    const glm::vec3 entryNormal = getNormal(entryIntersection.first, entryIntersection.second, id); // glm::vec3(lensMesh->getNormalModelTrans() * glm::vec4(getNormal(entryIntersection.first, entryIntersection.second), 0.f));
    const glm::vec3 entryRefraction = glm::normalize(cust::refract(entryRay.m_dir, entryNormal, m_ETA));

    // now let's find the next intersection:
    const std::pair<std::size_t, glm::vec3> passIntersection = nearestIntersectionMesh(lensMesh, Ray(entryIntersection.second, entryRefraction), entryIntersection.first);
    if (passIntersection.first >= lensMesh->getNumMeshFaces()) { return false; }

    // assign the line:
    result.m_pass = Line(entryIntersection.second, passIntersection.second);

    // now calulcate the next part:
    const MeshFace& passFace = lensMesh->getMeshFace(passIntersection.first);
    const glm::vec3 passNormal = -getNormal(passIntersection.first, passIntersection.second, id); //glm::vec3(lensMesh->getNormalModelTrans() * glm::vec4(getNormal(passIntersection.first, passIntersection.second), 0.f));
    const glm::vec3 passRefraction = glm::normalize(cust::refract(entryRefraction, passNormal, m_invETA));
    // TODO: fix the weirdness that is happening

    // now set the last part:
    result.m_end = Ray(passIntersection.second, passRefraction);

    *o_rayPath = result;
    return result.m_end.m_dir != glm::vec3();
}

glm::vec3 ee::RayTracer::getNormal(int triangle, glm::vec3 interPoint, unsigned id)
{
    Mesh* const lensMesh = m_lens.getMesh();
    const MeshFace& face = lensMesh->getMeshFace(triangle);

    glm::vec3 trianglePoints[3]; // get all of the points of the triangle: // don't bother with this
    for (int i = 0; i < 3; i++) { trianglePoints[i] = transPoint3(lensMesh->getModelTrans(), lensMesh->getVertex(face.m_indices[i]).m_position); }

    glm::vec4 t = lensMesh->getModelTrans() * glm::vec4(lensMesh->getVertex(face.m_indices[0]).m_position, 1.f);

    bool isCap = false; // check whether or not it is a cap
    for (int i = 0; i < 3; i++) { isCap = (face.m_indices[i] == 0 || face.m_indices[i] == (lensMesh->getNumVertices() - 1)) || isCap; }

    if (isCap)
    {
        return lensMesh->getNormal(triangle);
    }
    else
    {
        // stores the latitude and longitude spline ids:
        int latID[3];
        int lonID[3];
        for (int i = 0; i < 3; i++)
        {
            latID[i] = m_lens.getLatitudeIndex(face.m_indices[i]);
            lonID[i] = m_lens.getLongitudeIndex(face.m_indices[i]);

            if (latID[i] > m_lens.getConstraintStart() && latID[i] < m_lens.getConstraintEnd()) // in muslce
            {
                return glm::vec3(); // 0 vector normal, no can do if this occurs
            }
        }

        // Get the pairs of points that share a common spline:
        int latTriPointID_pair[2];
        int lonTriPointID_pair[2];
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
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

        // Get the odd point out that isn't shared amongst them
        int latTriPointID_single = 4;
        int lonTriPointID_single = 4;
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

        const bool isFirstHalf = lonID[lonTriPointID_pair[0]] < (m_lens.getNumLongitudes() / 2);

        // Build the splines:
        const bool isBottom = latID[0] < m_lens.getConstraintStart(); // defined as being below the start constraint
        buildLatSpline(latID[latTriPointID_pair[0]]);
        buildLatSpline(latID[latTriPointID_single]);
        buildLonSpline(lonID[lonTriPointID_pair[0]], isBottom);
        buildLonSpline(lonID[lonTriPointID_single], isBottom);

        // the point that comes first is always the point with the lowest 
        // index (simple, but there is one issue)
        // If one is equal to the greatest value and one is equal to the smallest value
        int latSplinePointID_start = glm::min(lonID[latTriPointID_pair[0]], lonID[latTriPointID_single]);
        int latSplinePointID_end   = glm::max(lonID[latTriPointID_pair[0]], lonID[latTriPointID_single]);
        if (latSplinePointID_end - latSplinePointID_start != 1)
        {
            latSplinePointID_start = m_lens.getNumLongitudes() - 1;
            latSplinePointID_end = m_lens.getNumLongitudes();
        }

        // it is a bit more difficult to get it working for the other situation
        // this is because it isn't ordered numerically as appropriate.
        // but this can be figured out based on it's "side"

        // This depends on whether or not is is on the bottom
        // NOTE: this is a bit (a lot...) of a hack, I WILL clean this up given the chance, maybe.
        int lonSplinePointID_start, lonSplinePointID_end;
        if (isBottom)
        {
            if (isFirstHalf) // then the greater index comes first
            {
                lonSplinePointID_start = m_lens.getConstraintStart() - glm::max(latID[lonTriPointID_pair[0]], latID[lonTriPointID_single]);
                lonSplinePointID_end = m_lens.getConstraintStart() - glm::min(latID[lonTriPointID_pair[0]], latID[lonTriPointID_single]);
            }
            else // the lower one goes first now
            {
                lonSplinePointID_start = m_lens.getConstraintStart() + 2 + glm::min(latID[lonTriPointID_pair[0]], latID[lonTriPointID_single]);
                lonSplinePointID_end = m_lens.getConstraintStart() + 2 + glm::max(latID[lonTriPointID_pair[0]], latID[lonTriPointID_single]);
            }
        }
        else
        {
            if (isFirstHalf) // then the greater index comes first
            {
                lonSplinePointID_start = glm::min(latID[lonTriPointID_pair[0]], latID[lonTriPointID_single]) - m_lens.getConstraintEnd();
                lonSplinePointID_end = glm::max(latID[lonTriPointID_pair[0]], latID[lonTriPointID_single]) - m_lens.getConstraintEnd();
            }
            else // the lower one goes first now
            {
                lonSplinePointID_start = m_lens.getNumLatitudes() - glm::max(latID[lonTriPointID_pair[0]], latID[lonTriPointID_single]) + m_lens.getNumLatitudes() - m_lens.getConstraintEnd();
                lonSplinePointID_end = m_lens.getNumLatitudes() - glm::min(latID[lonTriPointID_pair[0]], latID[lonTriPointID_single]) + m_lens.getNumLatitudes() - m_lens.getConstraintEnd();
            }
        }
        // don't have to worry about the cap because that is handled elsewhere
        
        const double latParam0 = getLatSplineParameter(latID[latTriPointID_pair[0]], latSplinePointID_start, latSplinePointID_end, interPoint);
        const double latParam1 = getLatSplineParameter(latID[latTriPointID_single], latSplinePointID_start, latSplinePointID_end, interPoint);
        const double lonParam0 = getLonSplineParameter(lonID[lonTriPointID_pair[0]], lonSplinePointID_start, lonSplinePointID_end, interPoint, isBottom);
        const double lonParam1 = getLonSplineParameter(lonID[lonTriPointID_single], lonSplinePointID_start, lonSplinePointID_end, interPoint, isBottom);
        
        const glm::vec3 latTangent = getLatTangent(latID[latTriPointID_pair[0]], latParam0, latID[latTriPointID_single], latParam1, interPoint);
        const glm::vec3 lonTangent = getLonTangent(lonID[lonTriPointID_pair[0]], lonParam0, lonID[lonTriPointID_single], lonParam1, interPoint, isBottom);

        // TODO: debug info:
        int debugIndex = 3 * id;
        testNormals[debugIndex + 1]->setRay(Ray(interPoint, lonTangent), 1.f);
        testNormals[debugIndex + 2]->setRay(Ray(interPoint, latTangent), 1.f);

        const glm::vec3 splineNormal = glm::normalize(glm::cross(latTangent, lonTangent));
        const glm::vec3 roughNormal = glm::normalize(interPoint); // for some reason, for certain triangles, the resulting normal is completely off.
        const glm::vec3 result = glm::dot(splineNormal, roughNormal) < 0 ? -splineNormal : splineNormal;

        testNormals[debugIndex]->setRay(Ray(interPoint, -result), 1.f);

        return result; // glm::dot(splineNormal, faceNormal) >= 0 ? splineNormal : -splineNormal;
    }
}

void ee::RayTracer::buildLatSpline(int latID)
{
    assert(latID >= 0 && latID < m_lens.getNumLatitudes()); // Not a valid latID
    if (m_latSplines[latID].m_valid) { return; } // a valid spline does not need to be rebuilt

    const auto& latitudes = m_lens.getLatitudes(latID);
    std::vector<glm::dvec3> points; // will store the points for the spline to use:

    for (int i = 0; i < latitudes.size(); i++)
    {
        const glm::dvec3 p(transPoint3(m_lens.getMesh()->getModelTrans(), m_lens.getMesh()->getVertex(latitudes[i]).m_position));
        points.push_back(p);
    }

    alglib::real_2d_array data;
    data.setcontent(points.size(), 3, reinterpret_cast<const double*>(points.data()));
    // alglib::pspline3build(data, points.size(), 0, 0, m_latSplines[latID].m_spline);
    alglib::pspline3buildperiodic(data, points.size(), 1, 0, m_latSplines[latID].m_spline);
    m_latSplines[latID].m_valid = true;

    // need to add the last point so it all works out
    const glm::dvec3 p(transPoint3(m_lens.getMesh()->getModelTrans(), m_lens.getMesh()->getVertex(latitudes[0]).m_position));
    points.push_back(glm::dvec3(p));
    m_latSplines[latID].m_splinePoints = std::move(points);
    m_latSplines[latID].setEdgeLengths();
}

void ee::RayTracer::buildLonSpline(int lonID, bool isBottom)
{
    assert(lonID >= 0 && lonID < m_lens.getNumLongitudes()); // Not a valid latID
    std::vector<SplineCache>& lonSplines = isBottom ? m_lonSplinesBottom : m_lonSplinesTop;

    Mesh* const lensMesh  = m_lens.getMesh();
    
    int lonID0, lonID1;
    lonID0 = normalizeLonID(lonID, &lonID1);

    if (lonSplines[lonID0].m_valid) { return; }

    auto& longitudes0 = m_lens.getLongitudes(lonID0);
    auto& longitudes1 = m_lens.getLongitudes(lonID1);

    const int startPoint0 = isBottom ? m_lens.getConstraintStart() : m_lens.getConstraintEnd();
    const int endPoint0   = isBottom ? 0 : longitudes0.size() - 1;
    const int startPoint1 = isBottom ? 0 : longitudes1.size() - 1;
    const int endPoint1   = startPoint0;

    const int endVertexID = isBottom ? 0 : lensMesh->getNumVertices() - 1;
    const int dec = isBottom ? -1 : 1;

    std::vector<glm::dvec3> points;
    int vertexIDID = startPoint0 - dec; // this is to offset the value so we can reach the correct point
    do
    {
        vertexIDID = vertexIDID + dec;
        const glm::dvec3 p(transPoint3(lensMesh->getModelTrans(), lensMesh->getVertex(longitudes0[vertexIDID]).m_position));
        points.push_back(p);
    } 
    while (vertexIDID != endPoint0);

    glm::dvec3 p(transPoint3(lensMesh->getModelTrans(), lensMesh->getVertex(endVertexID).m_position));
    points.push_back(p);
    vertexIDID = startPoint1 + dec; // same thing here

    do
    {
        vertexIDID = vertexIDID - dec;
        const glm::dvec3 p(transPoint3(lensMesh->getModelTrans(), lensMesh->getVertex(longitudes1[vertexIDID]).m_position));
        points.push_back(p);
    } 
    while (vertexIDID != endPoint1);

    alglib::real_2d_array data;
    data.setcontent(points.size(), 3, reinterpret_cast<const double*>(points.data()));
    alglib::pspline3build(data, points.size(), 1, 0, lonSplines[lonID0].m_spline);
    lonSplines[lonID0].m_valid = true;
    lonSplines[lonID0].m_splinePoints = std::move(points);
    lonSplines[lonID0].setEdgeLengths();
}

int ee::RayTracer::normalizeLonID(int originalLonID, int* o_sisterLonID) const
{
    const int halfNumLon = m_lens.getNumLongitudes() / 2;
    const int isFirstHalf = originalLonID < halfNumLon;
    const int lonID0 = originalLonID - !isFirstHalf * halfNumLon;
    const int lonID1 = originalLonID + isFirstHalf * halfNumLon;
    if (o_sisterLonID != nullptr) { *o_sisterLonID = lonID1; }
    return lonID0;
}

double ee::RayTracer::getLatSplineParameter(int splineID, int latSplinePointID_start, int latSplinePointID_end, glm::dvec3 interPoint) const
{
    return getSplineParameter(splineID, latSplinePointID_start, latSplinePointID_end, interPoint, m_latSplines);
}

double ee::RayTracer::getLonSplineParameter(int splineID, int lonSplinePointID_start, int lonSplinePointID_end, glm::dvec3 interPoint, bool bottom) const
{
    const int newLonID = normalizeLonID(splineID);
    return getSplineParameter(newLonID, lonSplinePointID_start, lonSplinePointID_end, interPoint, bottom ? m_lonSplinesBottom : m_lonSplinesTop);
}

double ee::RayTracer::getSplineParameter(int splineID, int splinePointID_start, int splinePointID_end, glm::dvec3 interPoint, const std::vector<SplineCache>& splineList) const
{
    assert(splineList[splineID].m_valid);

    // get the points:
    const glm::dvec3 startPoint = splineList[splineID].m_splinePoints[splinePointID_start];
    const glm::dvec3 endPoint = splineList[splineID].m_splinePoints[splinePointID_end];

    // get the position vector:
    const glm::dvec3 edgeVector = endPoint - startPoint;
    const glm::dvec3 interVector = interPoint - startPoint;

    double ratio = glm::dot(interVector, glm::normalize(edgeVector)) / glm::length(edgeVector);

    double lengths = 0.;
    for (int i = 0; i < splinePointID_start; i++)
    {
        lengths += splineList[splineID].m_edgeLens[i];
    }
    lengths += splineList[splineID].m_edgeLens[splinePointID_start] * ratio;

    return lengths / splineList[splineID].m_totalEdgeLen;
}

glm::dvec3 ee::RayTracer::getLatTangent(int spline0, double param0, int spline1, double param1, glm::dvec3 interPoint) const
{
    return getTangent(spline0, param0, spline1, param1, interPoint, m_latSplines);
}

glm::dvec3 ee::RayTracer::getLonTangent(int spline0, double param0, int spline1, double param1, glm::dvec3 interPoint, bool bottom) const
{
    const int newSpline0 = normalizeLonID(spline0);
    const int newSpline1 = normalizeLonID(spline1);
    return getTangent(newSpline0, param0, newSpline1, param1, interPoint, bottom ? m_lonSplinesBottom : m_lonSplinesTop);
}

glm::dvec3 ee::RayTracer::getTangent(int spline0, double param0, int spline1, double param1, glm::dvec3 interPoint, const std::vector<SplineCache>& splineList) const
{
    assert(splineList[spline0].m_valid);
    assert(splineList[spline1].m_valid);

    double x, y, z;

    // get the points:
    alglib::pspline3calc(splineList[spline0].m_spline, param0, x, y, z);
    const glm::dvec3 point0(x, y, z);
    alglib::pspline3calc(splineList[spline1].m_spline, param1, x, y, z);
    const glm::dvec3 point1(x, y, z);
    // get the tangents:
    alglib::pspline3tangent(splineList[spline0].m_spline, param0, x, y, z);
    const glm::dvec3 tangent0(x, y, z);
    alglib::pspline3tangent(splineList[spline1].m_spline, param1, x, y, z);
    const glm::dvec3 tangent1 = glm::dot(tangent0, glm::dvec3(x, y, z)) >= 0 ? glm::dvec3(x, y, z) : -glm::dvec3(x, y, z); // make sure the tangents face in the same direction

    const glm::dvec3 edgeVector = glm::normalize(point1 - point0);
    const glm::dvec3 interVector = interPoint - point0;
    const glm::dvec3 tangentStart = (glm::dot(interVector, edgeVector) * edgeVector) + point0;

    // exten them out by one:
    const glm::dvec3 secondPoint0 = point0 + tangent0;
    const glm::dvec3 secondPoint1 = point1 + tangent1;
    const glm::dvec3 secondEdgeVector = glm::normalize(secondPoint1 - secondPoint0);
    const glm::dvec3 tangentEnd = (glm::dot(interVector, secondEdgeVector) * secondEdgeVector) + secondPoint0;

    return glm::normalize(tangentEnd - tangentStart); // this should be unit anyways, but nothing wrong with normalizing extra
}

double ee::RayTracer::lonSplineParameter(int splineID, int startLatID, int endLatID, float parameter, bool bottom)
{
    const int  halfNumLon = m_lens.getNumLongitudes() / 2;
    int splineID0, splineID1;
    const int startEndLatIDDiff = std::abs(startLatID - endLatID);
    if (startEndLatIDDiff == 1) // a normal situation
    {
        const int isFirstHalf = splineID < halfNumLon;
        splineID0 = splineID - !isFirstHalf * halfNumLon;
        splineID1 = splineID + isFirstHalf * halfNumLon;
    }
    else // a situation ast the seam
    {
        // in this case, it should work either way
        splineID0 = m_lonSplinesBottom.size() - 2;
        splineID1 = m_lonSplinesBottom.size() - 1;
    }

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

    int newStartLonID, newEndLonID;
    if (splineID < halfNumLon) // it is on the other side of the lateral side
    {
        newStartLonID = bottom ? m_lens.getConstraintStart() - startLatID : startLatID - m_lens.getConstraintEnd();
        newEndLonID = bottom ? m_lens.getConstraintEnd() - endLatID : endLatID - m_lens.getConstraintEnd();
    }
    else
    {
        newStartLonID = bottom ? m_lens.getConstraintStart() + 2 + startLatID : ((m_lens.getNumLatitudes() - 1) - m_lens.getConstraintEnd()) + 2 + ((m_lens.getNumLatitudes() - 1) - startLatID);
        newEndLonID = bottom ? m_lens.getConstraintStart() + 2 + endLatID : ((m_lens.getNumLatitudes() - 1) - m_lens.getConstraintEnd()) + 2 + ((m_lens.getNumLatitudes() - 1) - endLatID);
    }

    const int startParam = data[newStartLonID];
    const int endParam = data[newEndLonID];

    const int relativeParam = (endParam - startParam) * parameter;
    return relativeParam + startParam;
}

glm::vec3 ee::RayTracer::getLatParamValue(int splineID, double param)
{
    param = param < 0 ? 0 : param;
    double x, y, z;
    alglib::pspline3calc(m_latSplines[splineID].m_spline, param, x, y, z);
    return glm::vec3(x, y, z);
}

glm::vec3 ee::RayTracer::getLonParamValue(int splineID, double param, bool bottom)
{
    param = param < 0 ? 0 : param;

    const int halfNumLon = m_lens.getNumLongitudes() / 2;
    const int splineID0 = splineID < halfNumLon ? splineID : splineID - halfNumLon;

    // assert(m_lonSplinesBottom[splineID0].m_valid);

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

glm::vec3 ee::RayTracer::getLatParamTangent(int splineID, double param)
{
    param = param < 0 ? 0 : param;
    double x, y, z;
    alglib::pspline3tangent(m_latSplines[splineID].m_spline, param, x, y, z);
    return glm::vec3(x, y, z);
}

glm::vec3 ee::RayTracer::getLonParamTangent(int splineID, double param, bool bottom)
{
    param = param < 0 ? 0 : param;

    const int halfNumLon = m_lens.getNumLongitudes() / 2;
    const int splineID0 = splineID - !(splineID < halfNumLon) * halfNumLon;

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

glm::vec3 ee::RayTracer::getLatTangent(int startSplineID, int endSplineID, double startParam, double endParam, float lonRatio)
{
    assert(startSplineID != endSplineID);

    const glm::vec3 startBeginPoint = getLatParamValue(startSplineID, startParam);
    const glm::vec3 endBeginPoint = getLatParamValue(endSplineID, endParam);
    const glm::vec3 beginVector = endBeginPoint - startBeginPoint;

    const glm::vec3 startTangent = getLatParamTangent(startSplineID, startParam); assert(startTangent != glm::vec3());
    glm::vec3 endTangent = getLatParamTangent(endSplineID, endParam); assert(endTangent != glm::vec3());
    endTangent = glm::dot(endTangent, startTangent) < 0 ? -endTangent : endTangent;

    const glm::vec3 startEndPoint = startBeginPoint + startTangent;
    const glm::vec3 endEndPoint = endBeginPoint + endTangent;
    const glm::vec3 endVector = endEndPoint - startEndPoint;

    const glm::vec3 tangentPoint0 = glm::normalize(beginVector) * (lonRatio * glm::length(beginVector)) + startBeginPoint;
    const glm::vec3 tangentPoint1 = glm::normalize(endVector) * (lonRatio * glm::length(endVector)) + startEndPoint;

    return glm::normalize(tangentPoint1 - tangentPoint0);
}

glm::vec3 ee::RayTracer::getLonTangent(int startSplineID, int endSplineID, double startParam, double endParam, float latRatio, bool bottom)
{
    assert(startSplineID != endSplineID);

    const glm::vec3 startBeginPoint = getLonParamValue(startSplineID, startParam, bottom);
    const glm::vec3 endBeginPoint = getLonParamValue(endSplineID, endParam, bottom);

    const glm::vec3 startTangent = getLonParamTangent(startSplineID, startParam, bottom); assert(startTangent != glm::vec3());
    glm::vec3 endTangent = getLonParamTangent(endSplineID, endParam, bottom); assert(endTangent != glm::vec3());
    endTangent = glm::dot(endTangent, startTangent) < 0 ? -endTangent : endTangent;

    const glm::vec3 startEndPoint = startBeginPoint + startTangent;
    const glm::vec3 endEndPoint = endBeginPoint + endTangent;

    const glm::vec3 beginVector = endBeginPoint - startBeginPoint;
    const glm::vec3 endVector = endEndPoint - startEndPoint;

    const glm::vec3 tangentPoint0 = glm::normalize(beginVector) * (latRatio * glm::length(beginVector)) + startBeginPoint;
    const glm::vec3 tangentPoint1 = glm::normalize(endVector) * (latRatio * glm::length(endVector)) + startEndPoint;

    return glm::normalize(tangentPoint1 - tangentPoint0);
}

void ee::RayTracer::clearSplineCache()
{
    std::fill(m_latSplines.begin(), m_latSplines.end(), SplineCache());
    std::fill(m_lonSplinesTop.begin(), m_lonSplinesTop.end(), SplineCache());
    std::fill(m_lonSplinesBottom.begin(), m_lonSplinesBottom.end(), SplineCache());
}

glm::vec3 ee::RayTracer::raytrace(std::size_t pos, const float rInc, const float cInc)
{
    clearSplineCache();
    for (std::size_t i = 0; i < m_rayOrigins.size(); i++)
    {
        for (std::size_t j = 0; j < m_cachedPoints.size(); j++)
        {
            const Ray currRay(glm::vec3(m_cachedPoints[j].x, m_cachedPoints[j].y, m_rayOrigins[i].z), glm::vec3(0.f, 0.f, 1.f)); // m_cachedPoints[j] - m_rayOrigins[i]);
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