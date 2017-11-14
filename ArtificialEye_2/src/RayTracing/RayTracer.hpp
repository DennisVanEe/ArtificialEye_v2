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

        //struct SplineCache
        //{
        //    bool m_valid;

        //    std::vector<Float>         m_edgeLens;
        //    // The following is how this data is formatted:
        //    // - [0, 1] => length up to 1
        //    // - [1, 2] => length up to 2
        //    // - etc...
        //    // So max length is always the last parameter
        //    std::vector<Vec3>     m_splinePoints;
        //    alglib::pspline3interpolant m_spline;

        //    SplineCache() : m_valid(false) {}

        //    Float getTotalEdgeLength() const
        //    {
        //        return m_edgeLens.back();
        //    }

        //    Float getEdgeLengthUpTo(int splinePointID) const
        //    {
        //        if (splinePointID == 0) { return 0; }
        //        return m_edgeLens[splinePointID - 1];
        //    }

        //    void setEdgeLengths()
        //    {
        //        assert(!m_splinePoints.empty());

        //        // now set the lengtsh needed:
        //        Float totalEdgeLen = 0.0;
        //        for (int i = 0; i < m_splinePoints.size() - 1; i++)
        //        {
        //            const Float length = glm::length(m_splinePoints[i] - m_splinePoints[i + 1]);
        //            totalEdgeLen += length;
        //            m_edgeLens.push_back(totalEdgeLen);
        //        }
        //    }
        //};

    private:
        // for debugging:
        // DrawLine* testNormals[13];

        std::vector<DrawLine*> testNormals;

        RayTracer(std::vector<Vec3> positions, Lens sphere, RayTracerParam param);

        Vec3       raytrace(std::size_t pos, Float rInc, Float cInc);
        bool       lensRefract(Ray startRay, LensRayPath* o_rayPath, unsigned id); // TODO: remove "debug" ray id from the execution
        Vec3       getNormal(int triangle, Vec3 interPoint, unsigned id);
        // void       buildLatSpline(int latID);
        // void       buildLonSpline(int lonID, bool isBottom);
        // int        normalizeLonID(int originalLonID, int* o_sisterLonID = nullptr) const; // helper function for functions that require lon IDs. 
                                                                                         // The first part is 

        // Float      getLatSplineParameter(int splineID, int latSplinePointID_start, int latSplinePointID_end, Vec3 interPoint) const;
        // Float      getLonSplineParameter(int splineID, int lonSplinePointID_start, int lonSplinePointID_end, Vec3 interPoint, bool bottom) const;
        // Float      getSplineParameter(int splineID, int splinePointID_start, int splinePointID_end, Vec3 interPoint, const std::vector<SplineCache>& splineList) const;

        // Vec3       getLatTangent(int spline0, Float param0, int spline1, Float param1, Vec3 interPoint) const;
        // Vec3       getLonTangent(int spline0, Float param0, int spline1, Float param1, Vec3 interPoint, bool bottom) const;
        // Vec3       getTangent(int spline0, Float param0, int spline1, Float param1, Vec3 interPoint, const std::vector<SplineCache>& splineList) const;

        // Float    lonSplineParameter(int splineID, int startLatID, int endLatID, Float parameter, bool bottom);

        // Vec3 getLatParamValue(int splineID, Float param);
        // Vec3 getLonParamValue(int splineID, Float param, bool bottom);
        // Vec3 getLatParamTangent(int splineID, Float param);
        // Vec3 getLonParamTangent(int splineID, Float param, bool bottom);

        // Vec3 getLatTangent(int startSplineID, int endSplineID, Float startParam, Float endParam, Float lonRatio);
        // Vec3 getLonTangent(int startSplineID, int endSplineID, Float startParam, Float endParam, Float latRatio, bool bottom);

        const Float          m_ETA;
        const Float          m_invETA;
        const std::size_t    m_widthResolution;
        const std::size_t    m_heightResolution;

        Lens                 m_lens;

        // some definitions to make my life easier:
        // const int            AKIMA   = 0;
        // const int            CAT_ROM = 1;
        // const int            CUBIC   = 2;

        // intersection caches:
        std::vector<Vec3> m_rayOrigins;
        std::vector<Vec3> m_cachedPoints;

        // spline caches:
        // void clearSplineCache();
        // std::vector<SplineCache> m_latSplines;
        // std::vector<SplineCache> m_lonSplinesTop;
        // std::vector<SplineCache> m_lonSplinesBottom;

        std::vector<Vec3>       m_resultColors; // the colors that will result
        std::vector<DrawLensRayPath> m_drawableLines; // for rendering (these are all the rays to draw)
    };
}