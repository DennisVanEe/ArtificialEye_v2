#include "Intersections.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

std::pair<bool, ee::Vector3> ee::intersectTriangle(Ray ray, Vector3 p0, Vector3 p1, Vector3 p2)
{
    Vector3 u, v, n;              // triangle vectors
    Vector3 dir, w0, w;           // ray vectors
    float r, a, b;              // params to calc ray-plane intersect

                                    // get triangle edge vectors and plane normal
    u = p1 - p0;
    v = p2 - p0;
    n = glm::cross(u, v);              // cross product
    if (n == Vector3())             // triangle is degenerate
        return std::make_pair(false, Vector3());                  // do not deal with this case

    dir = ray.m_dir;              // ray direction vector
    w0 = ray.m_origin - p0;
    a = -glm::dot(n, w0);
    b = glm::dot(n, dir);
    if (fabs(b) < 1e-8) 
    {     // ray is  parallel to triangle plane
        return std::make_pair(false, Vector3());  // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.f)                    // ray goes away from triangle
        return std::make_pair(false, Vector3()); // => no intersect
                                    // for a segment, also test if (r > 1.0) => no intersect

    Vector3 point = ray.m_origin + r * dir;            // intersect point of ray and plane

                                    // is I inside T?
    float uu, uv, vv, wu, wv, D;
    uu = glm::dot(u, u);
    uv = glm::dot(u, v);
    vv = glm::dot(v, v);
    w = point - p0;
    wu = glm::dot(w, u);
    wv = glm::dot(w, v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)         // I is outside T
        return std::make_pair(false, Vector3());
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return std::make_pair(false, Vector3());

    return std::make_pair(true, point);
}

std::pair<std::size_t, ee::Vector3> ee::nearestIntersection(const Mesh* mesh, Ray ray, std::size_t ignore)
{
    std::size_t minFace = mesh->getNumMeshFaces();
    Vector3 minPoint; // the point where the intersection would occur
    float minDist = std::numeric_limits<float>::max();

    for (std::size_t i = 0; i < mesh->getNumMeshFaces(); i++)
    {
        if (i != ignore)
        {
            const MeshFace& face = mesh->getMeshFace(i);

            Vector3 p0 = mesh->getVertex(face.m_indices[0]).m_position;
            Vector3 p1 = mesh->getVertex(face.m_indices[1]).m_position;
            Vector3 p2 = mesh->getVertex(face.m_indices[2]).m_position;

            auto result = intersectTriangle(ray, p0, p1, p2);
            if (result.first) // there was an intersection
            {
                glm::vec3 diff = result.second - ray.m_origin;
                if (glm::length2(glm::normalize(diff) - ray.m_dir) < FLT_EPSILON)
                {
                    float len = glm::length(diff); // find the length
                    if (len < minDist)
                    {
                        minDist = len;
                        minFace = i;
                        minPoint = result.second;
                    }
                }
            }
        }
    }

    return std::make_pair(minFace, minPoint);
}

glm::vec3 reftact(const glm::vec3 &I, const glm::vec3 &N, const float eta)
{
    float k = 1.0 - eta * eta * (1.0 - glm::dot(N, I) * glm::dot(N, I));
    if (k < 0.0)
        return glm::vec3();
    else
        return eta * I - (eta * glm::dot(N, I) + sqrt(k)) * N;
}

std::vector<ee::Ray> ee::meshRefract(const Mesh* const mesh, const Ray ray, const float eta)
{
    // Write it so that it loops until there are no more positive (forward) intersections
    // TODO -^

    Ray outRay;
    Ray inRay;
    inRay.m_dir = glm::normalize(ray.m_dir);
    inRay.m_origin = ray.m_origin;
    outRay = inRay;

    std::vector<Ray> results;

    for (int i = 0; i < 2; i++)
    {
        auto triangleIntersection = nearestIntersection(mesh, inRay);
        if (triangleIntersection.first >= mesh->getNumMeshFaces())
            break;

        int multFactor = (i & 1) * -2 + 1;
        float actualETA = (i & 1) ? 1.f / eta : eta;
       
        const auto& face = mesh->getMeshFace(triangleIntersection.first);
        glm::vec3 normal = mesh->getNormal(triangleIntersection.first);
        normal *= multFactor;
        glm::vec3 intersectionPt = triangleIntersection.second;

        glm::vec3 refractRay = refract(inRay.m_dir, normal, actualETA);

        outRay.m_dir = glm::normalize(refractRay);
        outRay.m_origin = intersectionPt;
        results.push_back(outRay);

        inRay = outRay;
    }

    return std::move(results);

    //Ray initialRay;
    //initialRay.m_dir = glm::normalize(ray.m_dir);
    //initialRay.m_origin = ray.m_origin;

    //// first one:
    //auto triangle0ID = nearestIntersection(mesh, initialRay);
    //if (triangle0ID.first == mesh->getNumMeshFaces())
    //{
    //    return initialRay;
    //}

    //const MeshFace& triangle0 = mesh->getMeshFace(triangle0ID.first);

    //// get the normal:
    //Vector3 v0 = mesh->getVertex(triangle0.m_indices[0]).m_position;
    //Vector3 v1 = mesh->getVertex(triangle0.m_indices[1]).m_position;
    //Vector3 v2 = mesh->getVertex(triangle0.m_indices[2]).m_position;
    //Vector3 e0 = v1 - v0;
    //Vector3 e1 = v2 - v0;
    //Vector3 initialNorm = glm::normalize(glm::cross(e0, e1));
    //Vector3 initialIntPoint = triangle0ID.second;

    //Vector3 initialRefract = glm::refract(initialRay.m_dir, initialNorm, 1.f / 1.66f);

    //Ray internalRay;
    //internalRay.m_dir = glm::normalize(initialRefract);
    //internalRay.m_origin = initialIntPoint;

    //// return internalRay;

    //auto triangle1ID = nearestIntersection(mesh, initialRay, triangle0ID.first);
    //if (triangle1ID.first == mesh->getNumMeshFaces())
    //{
    //    return internalRay;
    //}

    //const MeshFace& triangle1 = mesh->getMeshFace(triangle1ID.first);

    //// get the normal:
    //v0 = mesh->getVertex(triangle0.m_indices[0]).m_position;
    //v1 = mesh->getVertex(triangle0.m_indices[1]).m_position;
    //v2 = mesh->getVertex(triangle0.m_indices[2]).m_position;

    //e0 = v1 - v0;
    //e1 = v2 - v0;
    //Vector3 internalNorm = glm::normalize(glm::cross(e1, e0));
    //Vector3 internalIntPoint = triangle1ID.second;

    //Vector3 internalRefract = glm::refract(initialRay.m_dir, initialNorm, - 1.f / 1.66f);

    //Ray finalRay;
    //finalRay.m_dir = glm::normalize(internalRefract);
    //finalRay.m_origin = internalIntPoint;

    //return finalRay;
}