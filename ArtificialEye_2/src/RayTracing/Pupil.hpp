#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <random>
#include <ctime>
#include "../Types.hpp"

namespace ee
{
    class Pupil
    {
    public:
        Pupil(glm::mat4 pos, float startRadius = 0.f) :
            radius(startRadius), pos(pos) {}

        // Sampled points over pupil that don't vary
        void generateSamples(int numSamples)
        {
            std::random_device randDev;
            std::mt19937 generator(randDev());
            std::uniform_real_distribution<float> randDist(0.f, 1.f);

            m_samples.clear();
            m_samples.reserve(numSamples);
            for (int i = 0; i < numSamples; i++)
            {
                float val0 = randDist(generator);
                float val1 = randDist(generator);
                float val2 = randDist(generator);

                float t = PI2 * val0;
                float u = val1 + val2;
                float r = u > 1.f ? 2.f - u : u;

                const glm::vec3 point = glm::vec3(pos * glm::vec4(r * std::cos(t), r * std::sin(t), 0.f, 1.f));
                m_samples.push_back(point);
            }
        }

        const std::vector<glm::vec3>* getSamples() const
        {
            return &m_samples;
        }

        glm::mat4 pos;
        float     radius;

    private:
        std::vector<glm::vec3> m_samples;
    };
}