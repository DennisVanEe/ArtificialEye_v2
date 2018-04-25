#include "RTUtility.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

bool ee::intersectTriangle(Ray ray, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3* intpoint)
{
    const float EPS = 1e-8f;

    const glm::vec3 edge0 = p1 - p0;
    const glm::vec3 edge1 = p2 - p0;
    const glm::vec3 h = glm::cross(ray.dir, edge1);
    const float a = glm::dot(edge0, h); // determinent
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
    if (t > EPS) // ray intersection
    {
        const glm::vec3 outIntersectionPoint = ray.origin + (ray.dir * t);
        *intpoint = outIntersectionPoint;
        return true;
    }
    else
    {
        return false;
    }
}

void ee::baryCentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, float* u, float* v, float* w)
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

glm::vec3 ee::getNormal(const Mesh* mesh, int triangle, glm::vec3 interPoint)
{
	//return mesh->getTransformedFaceNormal(triangle);

	MeshFace face = mesh->getFace(triangle);
    glm::vec3 vert0 = mesh->getTransformedVertex(face[0]);
    glm::vec3 vert1 = mesh->getTransformedVertex(face[1]);
    glm::vec3 vert2 = mesh->getTransformedVertex(face[2]);

	float u, v, w;
	baryCentric(interPoint, vert0, vert1, vert2, &u, &v, &w);

    glm::vec3 norm0 = mesh->getTransVertexNormal(face[0]);
    glm::vec3 norm1 = mesh->getTransVertexNormal(face[1]);
    glm::vec3 norm2 = mesh->getTransVertexNormal(face[2]);

	glm::vec3 normal = glm::normalize(norm0 * u + norm1 * v + norm2 * w);

	return normal;
}

glm::vec2 ee::sampleUnit(int dist, int angles, int distFact, int angleFact)
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

// The resulting intersection point is transformed
int ee::nearestIntersectionMesh(const Mesh* mesh, Ray ray, int ignore, glm::vec3* intpoint)
{
    int minFace = -1;
    glm::vec3 minPoint; // the point where the intersection would occur
    float minDist = std::numeric_limits<Float>::max();

    for (int i = 0; i < mesh->getNumFaces(); i++)
    {
        if (i != ignore)
        {
            const MeshFace& face = mesh->getFace(i);

            // transform those points:
            glm::vec3 p0 = transPoint3(mesh->getModelTrans(), mesh->getVertex(face[0]));
			glm::vec3 p1 = transPoint3(mesh->getModelTrans(), mesh->getVertex(face[1]));
			glm::vec3 p2 = transPoint3(mesh->getModelTrans(), mesh->getVertex(face[2]));

            glm::vec3 resultpoint;
            bool result = intersectTriangle(ray, p0, p1, p2, &resultpoint);
            if (result) // there was an intersection
            {
				glm::vec3 diff = resultpoint - ray.origin;
                float len = glm::length(diff); // find the length
                if (len < minDist)
                {
                    minDist  = len;
                    minFace  = i;
                    minPoint = resultpoint;
                }
            }
        }
    }

    *intpoint = minPoint;
    return minFace;
}

glm::vec3 ee::refract(const glm::vec3& I, const glm::vec3& N, float eta)
{
    float NdotI = glm::dot(N, I);
    float k = 1.f - eta * eta * (1.f - NdotI * NdotI);
    if (k < 0.f)
    {
        return glm::vec3(NaN);
    }
    return eta * I - (eta * NdotI + std::sqrt(k)) * N;
}