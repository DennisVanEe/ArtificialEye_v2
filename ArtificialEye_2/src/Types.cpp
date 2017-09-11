#include "Types.hpp"

glm::vec3 ee::removeZeroError(glm::vec3 vec)
{
    int result = !(glm::epsilonEqual(vec, glm::vec3(), glm::epsilon<float>()) != glm::bvec3(true, true, true));
    vec *= result;
    return vec;
}