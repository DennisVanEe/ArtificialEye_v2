#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/epsilon.hpp>
#include <fstream>
#include <iomanip>
#include <vector>

namespace ee
{
	const float NaN = std::numeric_limits<float>::quiet_NaN();

    using Float = float;
    using Vec4 = glm::tvec4<Float>;
    using Vec3 = glm::tvec3<Float>;
    using Vec2 = glm::tvec2<Float>;
    using Mat4 = glm::tmat4x4<Float>;

    const Float PI  = glm::pi<Float>();
    const Float PI2 = 2 * PI;

    struct Ray
    {
        glm::vec3 origin;
        glm::vec3 dir;

        Ray() {}
        Ray(Vec3 origin, Vec3 dir) : origin(origin), dir(dir) {}
    };

    struct Line
    {
        Vec3 start;
        Vec3 end;

        Line() {}
        Line(Vec3 start, Vec3 end) : start(start), end(end) {}
    };

    __forceinline void writeVertex(std::ofstream& of, glm::vec3 v)
    {
        of << "v " << std::setprecision(4) << v.x << " " << std::setprecision(4) << v.y << " " << std::setprecision(4) << v.z << std::endl;
    }

    __forceinline void writeToOBJFile(std::string dir, const std::vector<Line>& lines)
    {
        std::ofstream writer(dir, std::ofstream::out | std::ofstream::trunc);

        unsigned int counter = 0;
        for (const Line line : lines)
        {
            Line otherLine;
            otherLine.end = line.end + glm::vec3(0.05f, 0.f, 0.f);
            otherLine.start = line.start + glm::vec3(0.05f, 0.f, 0.f);

            writeVertex(writer, line.end);
            writeVertex(writer, line.start);
            writeVertex(writer, otherLine.end);
            writeVertex(writer, otherLine.start);

            writer << "f " << counter + 1 << " " << counter + 2 << " " << counter + 3 << " " << counter + 4 << std::endl;
            counter += 4;
        }
    }

    // Zeros the vector if it is close enough to zero
    __forceinline Vec3 zeroIfCloseVector(Vec3 vec)
    {
        int result = !(glm::epsilonEqual(vec, Vec3(), glm::epsilon<Float>()) != glm::bvec3(true, true, true));
        vec *= result;
        return vec;
    }

    // Translates a point direction
    __forceinline glm::vec3 transPoint3(const Mat4& mat, const Vec3& vec)
    {
        return glm::vec3(mat * glm::vec4(vec, 1.0));
    }

    // Translates a vector direction
    __forceinline glm::vec3 transVector3(const Mat4& mat, Vec3 vec)
    {
        return glm::vec3(mat * glm::vec4(vec, 0.0));
    }

    // Flips the vector if it doesn't face the same direction
    __forceinline glm::vec3 flipSameDir(const Vec3& toFlip, const Vec3& ref)
    {
        return glm::dot(toFlip, ref) >= 0 ? toFlip : -toFlip;
    }

    __forceinline glm::vec3 alignDir(const glm::vec3& change, const glm::vec3& ref)
    {
        return glm::dot(change, ref) >= 0.f ? change : -change;
    }
}