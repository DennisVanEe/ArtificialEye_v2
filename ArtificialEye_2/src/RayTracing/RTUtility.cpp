#include "RTUtility.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

std::pair<bool, glm::vec3> ee::intersectTriangle(Ray ray, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
{
    std::pair<bool, glm::vec3> badResult(false, glm::vec3());

    const glm::vec3 edge0 = p1 - p0;
    const glm::vec3 edge1 = p2 - p0;
    const glm::vec3 h = glm::cross(ray.m_dir, edge1);
    const float a = glm::dot(edge0, h);
    if (fabsf(a) < FLT_EPSILON)
    {
        return badResult;
    }

    const float f = 1 / a;
    const glm::vec3 s = ray.m_origin - p0;
    const float u = f * glm::dot(s, h);
    if (u < 0.f || u > 1.f)
    {
        return badResult;
    }

    const glm::vec3 q = glm::cross(s, edge0);
    const float v = f * glm::dot(ray.m_dir, q);
    if (v < 0.f || u + v > 1.f)
    {
        return badResult;
    }
    // At this stage we can compute t to find out where the intersection point is on the line.
    const float t = f * glm::dot(edge1, q);
    if (t > FLT_EPSILON) // ray intersection
    {
        const glm::vec3 outIntersectionPoint = ray.m_origin + (ray.m_dir * t);
        return std::make_pair(true, outIntersectionPoint);
    }
    else
    {
        return badResult;
    }
}

std::pair<std::size_t, glm::vec3> ee::nearestIntersectionMesh(const Mesh* mesh, Ray ray, std::size_t ignore)
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

            const auto result = intersectTriangle(ray, p0, p1, p2);
            if (result.first) // there was an intersection
            {
                glm::vec3 diff = result.second - ray.m_origin;
                const float len = glm::length(diff); // find the length
                if (len < minDist)
                {
                    minDist  = len;
                    minFace  = i;
                    minPoint = result.second;
                }
            }
        }
    }

    return std::make_pair(minFace, minPoint);
}

glm::vec3 ee::cust::refract(const glm::vec3& I, const glm::vec3& N, const float eta)
{
    float k = 1.f - eta * eta * (1.f - glm::dot(N, I) * glm::dot(N, I));
    if (k < 0.f)
    {
        return glm::vec3();
    }
    else
    {
        return eta * I - (eta * glm::dot(N, I) + sqrtf(k)) * N;
    }
}