#pragma once

#include <cstddef>
#include <vector>
#include <glm/glm.hpp>

#include "../Alglib/interpolation.h"

#include "../Rendering/Modeling/Mesh.hpp"
#include "../Types.hpp"
#include "../Rendering/Renderer.hpp"

#include "../Rendering/TexturePacks/LineUniColorTextPack.hpp"
#include "../Rendering/Modeling/DrawLine.hpp"
#include "../Rendering/UVMeshSphere.hpp"
#include "RTUtility.hpp"

namespace ee
{
    struct RayTracerParam
    {
        std::size_t m_widthResolution;
        std::size_t m_heightResolution;
        float       m_lensRefractiveIndex;
        float       m_enviRefractiveIndex;
        glm::vec3   m_rayColor;
    };

    class RayTracer
    {
    public:
        static RayTracer& initialize(std::vector<glm::vec3> positions, UVMeshSphere sphere, RayTracerParam param);

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

        struct SplineCache
        {
            bool m_valid;
            alglib::pspline3interpolant m_spline;

            SplineCache() : m_valid(false) {}
        };

    private:
        RayTracer(std::vector<glm::vec3> positions, UVMeshSphere sphere, RayTracerParam param);

        glm::vec3 raytrace(std::size_t pos, float rInc, float cInc);
        bool lensRefract(Ray startRay, LensRayPath* o_rayPath);
        glm::vec3 getNormal(std::size_t triangle, glm::vec3 interPoint);
        void buildLatSpline(unsigned latID);
        void buildLonSpline(unsigned lonID, bool bottom);
        double latSplineParameter(unsigned splineID, unsigned startLonID, unsigned endLonID, float parameter);
        double lonSplineParameter(unsigned splineID, unsigned startLatID, unsigned endLatID, float parameter, bool bottom);

        glm::vec3 getLatParamValue(unsigned splineID, double param);
        glm::vec3 getLonParamValue(unsigned splineID, double param, bool bottom);

        const float    m_ETA;
        const float    m_invETA;
        const std::size_t    m_widthResolution;
        const std::size_t    m_heightResolution;

        UVMeshSphere   m_lens;

        // caches for higher performances:
        std::vector<glm::vec3> m_rayOrigins;
        std::vector<glm::vec3> m_cachedPoints;

        std::vector<SplineCache> m_latSplines;
        std::vector<SplineCache> m_lonSplinesTop;
        std::vector<SplineCache> m_lonSplinesBottom;

        std::vector<glm::vec3> m_resultColors; // the colors that will result
        std::vector<DrawLensRayPath> m_drawableLines; // for rendering
    };
}