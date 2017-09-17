#pragma once

#include <cstddef>
#include <vector>
#include <glm/glm.hpp>

#include "../Modeling/Mesh.hpp"
#include "../../Types.hpp"
#include "../Renderer.hpp"

#include "../TexturePacks/LineUniColorTextPack.hpp"
#include "../Modeling/DrawLine.hpp"
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
        struct DrawLensRayPath
        {
            DrawLine m_begin;
            DrawLine m_middle;
            DrawLine m_end;

            void sendToDrawable();

            DrawLensRayPath(std::string textPack);
        };

        struct LensRayPath
        {
            Line m_entry;
            Line m_pass;
            Ray  m_end;
        };

    private:
        RayTracer(std::vector<glm::vec3> positions, const Mesh* lens, float lensRefractiveIndex, RayTracerParam param, glm::vec3 rayColor);

        const Mesh* const m_lens;
        const float m_ETA;
        const RayTracerParam m_parameters;

        glm::vec3 raytrace(std::size_t pos, float rInc, float cInc);
        
        bool lensRefract(Ray startRay, LensRayPath* o_rayPath);

        std::vector<glm::vec3>        m_rayOrigins;
        std::vector<glm::vec3>        m_resultColors; // the colors that will result
        std::vector<glm::vec3>        m_cachedPoints;
        std::vector<DrawLensRayPath>  m_drawableLines; // for rendering
    };
}