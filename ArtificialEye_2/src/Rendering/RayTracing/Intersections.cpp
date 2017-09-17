#include "Intersections.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

std::pair<bool, glm::vec3> ee::intersectTriangle(Ray ray, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
{
    glm::vec3 u, v, n;              // triangle vectors
    glm::vec3 dir, w0, w;           // ray vectors
    float r, a, b;              // params to calc ray-plane intersect

                                    // get triangle edge vectors and plane normal
    u = p1 - p0;
    v = p2 - p0;
    n = glm::cross(u, v);              // cross product
    if (n == glm::vec3())             // triangle is degenerate
        return std::make_pair(false, glm::vec3());                  // do not deal with this case

    dir = ray.m_dir;              // ray direction vector
    w0 = ray.m_origin - p0;
    a = -glm::dot(n, w0);
    b = glm::dot(n, dir);
    if (fabs(b) < FLT_EPSILON) 
    {     // ray is  parallel to triangle plane
        return std::make_pair(false, glm::vec3());  // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.f)                    // ray goes away from triangle
    {
        return std::make_pair(false, glm::vec3()); // => no intersect
                                    // for a segment, also test if (r > 1.0) => no intersect
    }

    glm::vec3 point = ray.m_origin + r * dir;            // intersect point of ray and plane

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
        return std::make_pair(false, glm::vec3());
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return std::make_pair(false, glm::vec3());

    return std::make_pair(true, point);
}

std::pair<std::size_t, glm::vec3> ee::nearestIntersection(const Mesh* mesh, Ray ray, std::size_t ignore)
{
    std::size_t minFace = mesh->getNumMeshFaces();
    glm::vec3 minPoint; // the point where the intersection would occur
    float minDist = FLT_MAX;

    for (std::size_t i = 0; i < mesh->getNumMeshFaces(); i++)
    {
        if (i != ignore)
        {
            const MeshFace& face = mesh->getMeshFace(i);

            // transform those points:
            glm::vec3 p0 = glm::vec3(mesh->getModelTrans() * glm::vec4(mesh->getVertex(face.m_indices[0]).m_position, 1.f));
            glm::vec3 p1 = glm::vec3(mesh->getModelTrans() * glm::vec4(mesh->getVertex(face.m_indices[1]).m_position, 1.f));
            glm::vec3 p2 = glm::vec3(mesh->getModelTrans() * glm::vec4(mesh->getVertex(face.m_indices[2]).m_position, 1.f));

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

glm::vec3 ee::cust::refract(const glm::vec3& I, const glm::vec3& N, const float eta)
{
    float k = 1.0 - eta * eta * (1.0 - glm::dot(N, I) * glm::dot(N, I));
    if (k < 0.0)
    {
        return glm::vec3();
    }
    else
    {
        return eta * I - (eta * glm::dot(N, I) + sqrtf(k)) * N;
    }
}