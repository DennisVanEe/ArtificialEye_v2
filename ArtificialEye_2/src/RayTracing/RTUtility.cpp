#include "RTUtility.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

std::pair<bool, ee::Vec3> ee::intersectTriangle(Ray ray, Vec3 p0, Vec3 p1, Vec3 p2)
{
    const Float EPS = glm::epsilon<Float>(); // 1e-8;

    std::pair<bool, Vec3> badResult(false, Vec3());

    const Vec3 edge0 = p1 - p0;
    const Vec3 edge1 = p2 - p0;
    const Vec3 h = glm::cross(ray.m_dir, edge1);
    const Float a = glm::dot(edge0, h); // determinent
    if (std::abs(a) < EPS)
    {
        return badResult;
    }

    const Float f = 1.0 / a;
    const Vec3 s = ray.m_origin - p0;
    const Float u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0)
    {
        return badResult;
    }

    const Vec3 q = glm::cross(s, edge0);
    const Float v = f * glm::dot(ray.m_dir, q);
    if (v < 0.0 || u + v > 1.0)
    {
        return badResult;
    }
    // At this stage we can compute t to find out where the intersection point is on the line.
    const Float t = f * glm::dot(edge1, q);
    if (t > EPS) // ray intersection
    {
        const Vec3 outIntersectionPoint = ray.m_origin + (ray.m_dir * t);
        return std::make_pair(true, outIntersectionPoint);
    }
    else
    {
        return badResult;
    }
}

void ee::baryCentric(Vec3 p, Vec3 a, Vec3 b, Vec3 c, Float &u, Float &v, Float &w)
{
    Vec3 v0 = b - a, v1 = c - a, v2 = p - a;
    Float d00 = glm::dot(v0, v0);
    Float d01 = glm::dot(v0, v1);
    Float d11 = glm::dot(v1, v1);
    Float d20 = glm::dot(v2, v0);
    Float d21 = glm::dot(v2, v1);
    Float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0 - v - w;
}

// The resulting intersection point is transformed
std::pair<std::size_t, ee::Vec3> ee::nearestIntersectionMesh(const Mesh* mesh, Ray ray, std::size_t ignore)
{
    std::size_t minFace = mesh->getNumMeshFaces();
    Vec3 minPoint; // the point where the intersection would occur
    Float minDist = std::numeric_limits<Float>::max();

    for (std::size_t i = 0; i < mesh->getNumMeshFaces(); i++)
    {
        if (i != ignore)
        {
            const MeshFace& face = mesh->getMeshFace(i);

            // transform those points:
            Vec3 p0 = transPoint3(mesh->getModelTrans(), mesh->getVertex(face(0)).m_position);
            Vec3 p1 = transPoint3(mesh->getModelTrans(), mesh->getVertex(face(1)).m_position);
            Vec3 p2 = transPoint3(mesh->getModelTrans(), mesh->getVertex(face(2)).m_position);

            const auto result = intersectTriangle(ray, p0, p1, p2);
            if (result.first) // there was an intersection
            {
                Vec3 diff = result.second - ray.m_origin;
                const Float len = glm::length(diff); // find the length
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

ee::Vec3 ee::cust::refract(const Vec3& I, const Vec3& N, const Float eta)
{
    const Float NdotI = glm::dot(N, I);
    const Float k = 1.0 - eta * eta * (1.0 - NdotI * NdotI);
    if (k < 0.0)
    {
        return Vec3();
    }
    return eta * I - (eta * NdotI + std::sqrt(k)) * N;
}