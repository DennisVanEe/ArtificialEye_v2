#pragma once

#include "../../Types.hpp"

namespace ee
{
    struct Ray
    {
        Vector3 m_origin;
        Vector3 m_dir;

        Ray() {}
        Ray(Vector3 origin, Vector3 dir) : m_origin(origin), m_dir(dir) {}
    };
}