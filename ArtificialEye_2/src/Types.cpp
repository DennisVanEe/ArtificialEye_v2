#include "Types.hpp"

glm::vec3 ee::removeZeroError(glm::vec3 vec)
{
    int result = !(glm::epsilonEqual(vec, glm::vec3(), glm::epsilon<float>()) != glm::bvec3(true, true, true));
    vec *= result;
    return vec;
}

ee::Ray::Ray(glm::vec3 origin, glm::vec3 dir) : 
    m_origin(origin), 
    m_dir(dir) 
{
}

ee::Line::Line(glm::vec3 start, glm::vec3 end) : 
    m_start(start), 
    m_end(end) 
{
}

glm::vec3 ee::transPoint3(const glm::mat4& mat, glm::vec3 vec)
{
    return glm::vec3(mat * glm::vec4(vec, 1.f));
}

glm::vec3 ee::transVector3(const glm::mat4& mat, glm::vec3 vec)
{
    return glm::vec3(mat * glm::vec4(vec, 0.f));
}
