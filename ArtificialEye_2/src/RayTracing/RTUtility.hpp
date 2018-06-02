#pragma once

#include <limits>
#include <utility>
#include <random>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../Types.hpp"
#include "../Mesh/Mesh.hpp"

namespace ee
{
    // returns whether or not there is an intersection and where (point)
    __forceinline bool intersectTriangle(Ray ray, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3* intpoint)
    {
        static const float EPS = 1e-8f;

        const glm::vec3 edge0 = p1 - p0;
        const glm::vec3 edge1 = p2 - p0;
        const glm::vec3 h = glm::cross(ray.dir, edge1);
        const float a = glm::dot(edge0, h);
        if (std::abs(a) < EPS)
        {
            return false;
        }

        const float f = 1.f / a;
        const glm::vec3 s = ray.origin - p0;
        const float u = f * glm::dot(s, h);
        if (u < 0.f || u > 1.f)
        {
            return false;
        }

        const glm::vec3 q = glm::cross(s, edge0);
        const float v = f * glm::dot(ray.dir, q);
        if (v < 0.f || u + v > 1.f)
        {
            return false;
        }

        // At this stage we can compute t to find out where the intersection point is on the line.
        const float t = f * glm::dot(edge1, q);
        const glm::vec3 outIntersectionPoint = ray.origin + (ray.dir * t);
        *intpoint = outIntersectionPoint;

        return t > EPS;
    }

    __forceinline void baryCentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, float* u, float* v, float* w)
    {
        const glm::vec3 v0 = b - a;
        const glm::vec3 v1 = c - a;
        const glm::vec3 v2 = p - a;

        const float d00 = glm::dot(v0, v0);
        const float d01 = glm::dot(v0, v1);
        const float d11 = glm::dot(v1, v1);
        const float d20 = glm::dot(v2, v0);
        const float d21 = glm::dot(v2, v1);
        const float denom = d00 * d11 - d01 * d01;

        *v = (d11 * d20 - d01 * d21) / denom;
        *w = (d00 * d21 - d01 * d20) / denom;
        *u = 1.f - *v - *w;
    }

    __forceinline void baryCentric(glm::vec3 p, Triangle triangle, float* u, float* v, float* w)
    {
        baryCentric(p, triangle[0], triangle[1], triangle[2], u, v, w);
    }

	// generate a random number generator
	__forceinline glm::vec2 sampleUnit(int dist, int angles, int distFact, int angleFact)
    {
        const float angleFactor = static_cast<float>(angleFact);
        const float distanceFactor = static_cast<float>(distFact);

        float distanceIndex = static_cast<float>(dist);
        float angleIndex = static_cast<float>(angles);

        const float r = distanceIndex / distanceFactor;
        const float theta = (PI2 / angleFactor) * (angleIndex + r);

        const float x = r * std::cos(theta);
        const float y = r * std::sin(theta);

        return glm::vec2(x, y);
    }

    // custom refract function (glm's acts weirdly)
    __forceinline glm::vec3 refract(const glm::vec3& I, const glm::vec3& N, float eta)
    {
        const float NdotI = glm::dot(N, I);
        const float k = 1.f - eta * eta * (1.f - NdotI * NdotI);
        if (k < 0.f)
        {
            return glm::vec3(NaN);
        }
        return eta * I - (eta * NdotI + std::sqrt(k)) * N;
    }
}