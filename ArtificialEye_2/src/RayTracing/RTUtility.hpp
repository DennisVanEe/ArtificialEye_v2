#pragma once

#include <limits>
#include <utility>
#include <random>
#include <ctime>

#include "../Types.hpp"
#include "../Mesh/Mesh.hpp"

namespace ee
{
    // returns whether or not there is an intersection and where (point)
    bool intersectTriangle(Ray ray, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3* intpoint);
    // returns the nearest intersection (point) and which triangle in the mesh (if any)
    int nearestIntersectionMesh(const Mesh* mesh, Ray ray, int ignore, glm::vec3* intpoint);

    void baryCentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, float* u, float* v, float* w);

	glm::vec3 getNormal(const Mesh* mesh, int triangle, glm::vec3 interPoint);

	// generate a random number generator
	glm::vec2 sampleUnit(int dist, int angles, int distFact, int angleFact);

    // custom refract function (glm's acts weirdly)
    glm::vec3 refract(const glm::vec3& I, const glm::vec3& N, float eta);
}