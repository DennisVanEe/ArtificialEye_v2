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

            std::vector<float>          m_edgeLens; // used to find the overall parameter value
            alglib::pspline3interpolant m_spline;

            SplineCache() : m_valid(false) {}

            template<typename T>
            void setEdgeLengths(const std::vector<glm::tvec3<T>>& points)
            {
                // now set the lengtsh needed:
                for (int i = 0; i < points.size() - 1; i++)
                {
                    const float length = static_cast<float>(glm::length(points[i] - points[i + 1]));
                    m_edgeLens.push_back(length);
                }
            }
        };

    private:
        // for debugging:
        DrawLine* testNormals[13];

        RayTracer(std::vector<glm::vec3> positions, UVMeshSphere sphere, RayTracerParam param);

        glm::vec3 raytrace(std::size_t pos, float rInc, float cInc);
        bool      lensRefract(Ray startRay, LensRayPath* o_rayPath, unsigned id); // TODO: remove "debug" ray id from the execution
        glm::vec3 getNormal(int triangle, glm::vec3 interPoint, unsigned id);
        void      buildLatSpline(int latID);
        void      buildLonSpline(int lonID, bool bottom);
        double    latSplineParameter(int splineID, int startLonID, int endLonID, float parameter);
        double    lonSplineParameter(int splineID, int startLatID, int endLatID, float parameter, bool bottom);

        glm::vec3 getLatParamValue(int splineID, double param);
        glm::vec3 getLonParamValue(int splineID, double param, bool bottom);
        glm::vec3 getLatParamTangent(int splineID, double param);
        glm::vec3 getLonParamTangent(int splineID, double param, bool bottom);

        glm::vec3 getLatTangent(int startSplineID, int endSplineID, double startParam, double endParam, float lonRatio);
        glm::vec3 getLonTangent(int startSplineID, int endSplineID, double startParam, double endParam, float latRatio, bool bottom);

        const float          m_ETA;
        const float          m_invETA;
        const std::size_t    m_widthResolution;
        const std::size_t    m_heightResolution;

        UVMeshSphere         m_lens;

        // intersection caches:
        std::vector<glm::vec3> m_rayOrigins;
        std::vector<glm::vec3> m_cachedPoints;

        // spline caches:
        void clearSplineCache();
        std::vector<SplineCache> m_latSplines;
        std::vector<SplineCache> m_lonSplinesTop;
        std::vector<SplineCache> m_lonSplinesBottom;

        std::vector<glm::vec3>       m_resultColors; // the colors that will result
        std::vector<DrawLensRayPath> m_drawableLines; // for rendering (these are all the rays to draw)
    };
}