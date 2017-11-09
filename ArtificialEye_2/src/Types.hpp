#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/epsilon.hpp>

namespace ee
{
    const char PROJ_NAME[] = "ArtificalEye";

    using Float = double;
    using Vec4 = glm::tvec4<Float>;
    using Vec3 = glm::tvec3<Float>;
    using Vec2 = glm::tvec2<Float>;
    using Mat4 = glm::tmat4x4<Float>;

    const Float PI  = glm::pi<Float>();
    const Float PI2 = 2 * PI;

    struct Ray
    {
        Vec3 m_origin;
        Vec3 m_dir;

        Ray() {}
        Ray(Vec3 origin, Vec3 dir) : m_origin(origin), m_dir(dir) {}
    };

    struct Line
    {
        Vec3 m_start;
        Vec3 m_end;

        Line() {}
        Line(Vec3 start, Vec3 end) : m_start(start), m_end(end) {}
    };

    // Zeros the vector if it is close enough to zero
    Vec3 zeroVector(Vec3 vec)
    {
        int result = !(glm::epsilonEqual(vec, Vec3(), glm::epsilon<Float>()) != glm::bvec3(true, true, true));
        vec *= result;
        return vec;
    }

    // Translates a point direction
    Vec3 transPoint3(const Mat4& mat, const Vec3& vec)
    {
        return Vec3(mat * Vec4(vec, 1.f));
    }

    // Translates a vector direction
    Vec3 transVector3(const Mat4& mat, Vec3 vec)
    {
        return Vec3(mat * Vec4(vec, 0.f));
    }

    // Flips the vector if it doesn't face the same direction
    Vec3 flipSameDir(const Vec3& toFlip, const Vec3& ref)
    {
        return glm::dot(toFlip, ref) >= 0 ? toFlip : -toFlip;
    }
}