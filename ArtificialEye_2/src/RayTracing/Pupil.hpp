#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include "../Types.hpp"

namespace ee
{
    class Pupil
    {
    public:
        Pupil(glm::mat4 pos, float startRadius = 0.f) :
            radius(startRadius), pos(pos) {}

        // Sampled points over pupil that don't vary
        const std::vector<glm::vec3>* sampleFixed(int distFact, int angleFact)
        {
            m_fixedSamples.clear();
            m_fixedSamples.reserve(distFact * angleFact);
            for (int dist = 0; dist < distFact; dist++)
            {
                for (int angles = 0; angles < angleFact; angles++)
                {
                    const float angleFactor = static_cast<float>(angleFact);
                    const float distanceFactor = static_cast<float>(distFact);

                    float distanceIndex = static_cast<float>(dist);
                    float angleIndex = static_cast<float>(angles);

                    const float r = radius * distanceIndex / distanceFactor;
                    const float theta = (PI2 / angleFactor) * (angleIndex + r);

                    const float x = r * std::cos(theta);
                    const float y = r * std::sin(theta);

                    const glm::vec3 point = glm::vec3(pos * glm::vec4(x, y, 0.f, 1.f));
                    m_fixedSamples.push_back(point);
                }
            }

            return &m_fixedSamples;
        }

        const std::vector<glm::vec3>* getFixedSamples() const
        {
            return &m_fixedSamples;
        }

        glm::mat4 pos;
        float     radius;

    private:
        std::vector<glm::vec3> m_fixedSamples;
    };
}