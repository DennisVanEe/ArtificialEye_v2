#pragma once

#include <cstddef>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>

#include "../Alglib/interpolation.h"

#include "../Rendering/Modeling/Mesh.hpp"
#include "../Types.hpp"
#include "../Rendering/Renderer.hpp"

#include "../Rendering/TexturePacks/LineUniColorTextPack.hpp"
#include "../Rendering/Modeling/DrawLine.hpp"
#include "../Rendering/Lens.hpp"
#include "RTUtility.hpp"

namespace ee
{
    struct RayTracerParam
    {
        int         m_widthResolution;
        int         m_heightResolution;
        Float       m_lensRefractiveIndex;
        Float       m_enviRefractiveIndex;
        Vec3        m_rayColor;
    };

    class RayTracer
    {
    public:
        static RayTracer& initialize(std::vector<Vec3> positions, Lens sphere, RayTracerParam param);

        void raytrace();
        const std::vector<Vec3>& getResultColors() const;

    private:
        struct DrawLensRayPath
        {
            DrawLine m_begin;
            DrawLine m_middle;
            DrawLine m_end;

            void sendToDrawable()
            {
                ee::Renderer::addDrawable(&m_begin);
                ee::Renderer::addDrawable(&m_middle);
                ee::Renderer::addDrawable(&m_end);
            }

            DrawLensRayPath(std::string textPack) :
                m_begin(textPack, glm::vec3(), glm::vec3()),
                m_middle(textPack, glm::vec3(), glm::vec3()),
                m_end(textPack, glm::vec3(), glm::vec3()) {}
        };

        struct LensRayPath
        {
            Line m_entry;
            Line m_pass;
            Ray  m_end;
        };

    private:
        // for debugging:
        // DrawLine* testNormals[13];

        std::vector<DrawLine*> testNormals;

        RayTracer(std::vector<Vec3> positions, Lens sphere, RayTracerParam param);

        Vec3       raytrace(int pos, Float rInc, Float cInc);
        bool       lensRefract(Ray startRay, LensRayPath* o_rayPath, unsigned id); // TODO: remove "debug" ray id from the execution
        Vec3       getNormal(int triangle, Vec3 interPoint, unsigned id);

        const Float          m_ETA;
        const Float          m_invETA;
        const std::size_t    m_widthResolution;
        const std::size_t    m_heightResolution;

        Lens                 m_lens;

        // intersection caches:
        std::vector<Vec3> m_rayOrigins;
        std::vector<Vec3> m_cachedPoints;

        std::vector<Vec3>            m_resultColors; // the colors that will result
        std::vector<DrawLensRayPath> m_drawableLines; // for rendering (these are all the rays to draw)
    };
}