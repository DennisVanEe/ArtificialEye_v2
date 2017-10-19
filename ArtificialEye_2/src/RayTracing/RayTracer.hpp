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

            std::vector<double>         m_edgeLens;
            // The following is how this data is formatted:
            // - [0, 1] => length up to 1
            // - [1, 2] => length up to 2
            // - etc...
            // So max length is always the last parameter
            std::vector<glm::dvec3>     m_splinePoints;
            alglib::pspline3interpolant m_spline;

            SplineCache() : m_valid(false) {}

            double getTotalEdgeLength() const
            {
                return m_edgeLens.back();
            }

            double getEdgeLengthUpTo(int splinePointID) const
            {
                if (splinePointID == 0) { return 0; }
                return m_edgeLens[splinePointID - 1];
            }

            void setEdgeLengths()
            {
                assert(!m_splinePoints.empty());

                // now set the lengtsh needed:
                double totalEdgeLen = 0.0;
                for (int i = 0; i < m_splinePoints.size() - 1; i++)
                {
                    const double length = glm::length(m_splinePoints[i] - m_splinePoints[i + 1]);
                    totalEdgeLen += length;
                    m_edgeLens.push_back(totalEdgeLen);
                }
            }
        };

    private:
        // for debugging:
        // DrawLine* testNormals[13];

        std::vector<DrawLine*> testNormals;

        RayTracer(std::vector<glm::vec3> positions, UVMeshSphere sphere, RayTracerParam param);

        glm::vec3  raytrace(std::size_t pos, float rInc, float cInc);
        bool       lensRefract(Ray startRay, LensRayPath* o_rayPath, unsigned id); // TODO: remove "debug" ray id from the execution
        glm::vec3  getNormal(int triangle, glm::vec3 interPoint, unsigned id);
        void       buildLatSpline(int latID);
        void       buildLonSpline(int lonID, bool isBottom);
        int        normalizeLonID(int originalLonID, int* o_sisterLonID = nullptr) const; // helper function for functions that require lon IDs. 
                                                                                         // The first part is 

        double     getLatSplineParameter(int splineID, int latSplinePointID_start, int latSplinePointID_end, glm::dvec3 interPoint) const;
        double     getLonSplineParameter(int splineID, int lonSplinePointID_start, int lonSplinePointID_end, glm::dvec3 interPoint, bool bottom) const;
        double     getSplineParameter(int splineID, int splinePointID_start, int splinePointID_end, glm::dvec3 interPoint, const std::vector<SplineCache>& splineList) const;

        glm::dvec3 getLatTangent(int spline0, double param0, int spline1, double param1, glm::dvec3 interPoint) const;
        glm::dvec3 getLonTangent(int spline0, double param0, int spline1, double param1, glm::dvec3 interPoint, bool bottom) const;
        glm::dvec3 getTangent(int spline0, double param0, int spline1, double param1, glm::dvec3 interPoint, const std::vector<SplineCache>& splineList) const;

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

        // some definitions to make my life easier:
        const int            AKIMA   = 0;
        const int            CAT_ROM = 1;
        const int            CUBIC   = 2;

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