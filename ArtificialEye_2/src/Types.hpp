#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/epsilon.hpp>

namespace ee
{
    const char PROJ_NAME[] = "ArtificalEye";

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
        Vec3 origin;
        Vec3 dir;

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

    // Zeros the vector if it is close enough to zero
    inline Vec3 zeroIfCloseVector(Vec3 vec)
    {
        int result = !(glm::epsilonEqual(vec, Vec3(), glm::epsilon<Float>()) != glm::bvec3(true, true, true));
        vec *= result;
        return vec;
    }

    // Translates a point direction
    inline Vec3 transPoint3(const Mat4& mat, const Vec3& vec)
    {
        return Vec3(mat * Vec4(vec, 1.0));
    }

    // Translates a vector direction
    inline Vec3 transVector3(const Mat4& mat, Vec3 vec)
    {
        return Vec3(mat * Vec4(vec, 0.0));
    }

    // Flips the vector if it doesn't face the same direction
    inline Vec3 flipSameDir(const Vec3& toFlip, const Vec3& ref)
    {
        return glm::dot(toFlip, ref) >= 0 ? toFlip : -toFlip;
    }

    inline glm::vec3 alignDir(const glm::vec3& change, const glm::vec3& ref)
    {
        return glm::dot(change, ref) >= 0.f ? change : -change;
    }
}