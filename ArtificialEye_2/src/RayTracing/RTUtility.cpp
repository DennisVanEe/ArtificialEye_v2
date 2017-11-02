#include "RTUtility.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

std::pair<bool, glm::vec3> ee::intersectTriangle(Ray ray, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
{
    const float EPS = 1e-8;

    std::pair<bool, glm::vec3> badResult(false, glm::vec3());

    const glm::vec3 edge0 = p1 - p0;
    const glm::vec3 edge1 = p2 - p0;
    const glm::vec3 h = glm::cross(ray.m_dir, edge1);
    const float a = glm::dot(edge0, h); // determinent
    if (std::abs(a) < EPS)
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
    if (t > EPS) // ray intersection
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
            glm::vec3 p0 = transPoint3(mesh->getModelTrans(), mesh->getVertex(face.m_indices[0]).m_position);
            glm::vec3 p1 = transPoint3(mesh->getModelTrans(), mesh->getVertex(face.m_indices[1]).m_position);
            glm::vec3 p2 = transPoint3(mesh->getModelTrans(), mesh->getVertex(face.m_indices[2]).m_position);

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

                    //float minX = glm::min(glm::min(p0.x, p1.x), p2.x);
                    //float maxX = glm::max(glm::max(p0.x, p1.x), p2.x);
                    //assert(minX < result.second.x && result.second.x < maxX);

                    //float minY = glm::min(glm::min(p0.y, p1.y), p2.y);
                    //float maxY = glm::max(glm::max(p0.y, p1.y), p2.y);
                    //assert(minY < result.second.y && result.second.y < maxY);

                    //float minZ = glm::min(glm::min(p0.z, p1.z), p2.z);
                    //float maxZ = glm::max(glm::max(p0.z, p1.z), p2.z);
                    //assert(minZ < result.second.z && result.second.z < maxZ);
                }
            }
        }
    }

    return std::make_pair(minFace, minPoint);
}

glm::vec3 ee::cust::refract(const glm::vec3& I, const glm::vec3& N, const float eta)
{
    const float NdotI = glm::dot(N, I);
    const float k = 1.f - eta * eta * (1.f - NdotI * NdotI);
    if (k < 0.f)
    {
        return glm::vec3();
    }
    return eta * I - (eta * NdotI + sqrtf(k)) * N;
}