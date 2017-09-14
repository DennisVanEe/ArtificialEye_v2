#pragma once

#include <cstddef>
#include <vector>
#include <glm/glm.hpp>

#include "../Modeling/Mesh.hpp"
#include "Ray.hpp"
#include "../Renderer.hpp"

#include "../TexturePacks/LineUniColorTextPack.hpp"
#include "../Modeling/Line.hpp"
#include "Intersections.hpp"

namespace ee
{
    struct RayTracerParam
    {
        std::size_t m_widthResolution;
        std::size_t m_heightResolution;
    };

    class RayTracer
    {
    public:
        static RayTracer& initialize(std::vector<glm::vec3> positions, const Mesh* lens, float lensRefractiveIndex, RayTracerParam param, glm::vec3 rayColor);

        void raytrace();

        const std::vector<glm::vec3>& getResultColors() const;

    private:
        struct RayPack
        {
            Line m_begin;
            Line m_middle;
            Line m_end;

            void sendToDrawable()
            {
                ee::Renderer::addDrawable(&m_begin);
                ee::Renderer::addDrawable(&m_middle);
                ee::Renderer::addDrawable(&m_end);
            }

            RayPack(std::string textPack) :
                m_begin(textPack, glm::vec3(), glm::vec3()),
                m_middle(textPack, glm::vec3(), glm::vec3()),
                m_end(textPack, glm::vec3(), glm::vec3()) {}
        };

        struct LensRayPath
        {
            LineSegment m_entry;
            LineSegment m_pass;
            Ray         m_end;
        };

    private:
        RayTracer(std::vector<glm::vec3> positions, const Mesh* lens, float lensRefractiveIndex, RayTracerParam param, glm::vec3 rayColor);

        const Mesh* const m_lens;
        const float m_ETA;
        const RayTracerParam m_parameters;

        glm::vec3 raytrace(std::size_t pos, float rInc, float cInc);
        
        bool lensRefract(Ray startRay, LensRayPath* o_rayPath);

        std::vector<glm::vec3> m_rayOrigins;
        std::vector<glm::vec3> m_resultColors; // the colors that will result
        std::vector<glm::vec3> m_cachedPoints;
        std::vector<RayPack>   m_drawableLines; // for rendering
    };
}