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

#undef min
#undef max

namespace ee
{
    struct RayTracerParam
    {
        int         m_widthResolution;
        int         m_heightResolution;
        Float       m_lensRefractiveIndex_middle;
        Float       m_lensRefractiveIndex_end;
        Float       m_eyeballRefractiveIndex;
        Float       m_enviRefractiveIndex;
        Vec3        m_rayColor;
    };

    class RayTracer
    {
    public:
        static RayTracer& initialize(std::vector<Vec3> positions, Lens sphere, RayTracerParam param);

        void raytrace();
        const std::vector<Vec3>& getResultColors() const;

        void setCorneaSphere(Mat4 transform)
        {
            m_corneaSphere = transform;
            m_invCorneaSphere = glm::inverse(m_corneaSphere);
        }

    private:
        struct DrawLensRayPath
        {
            DrawLine m_airToCornea;
            DrawLine m_corneaToLens;
            DrawLine m_inLens;
            DrawLine m_lensToCornea;

            void sendToDrawable()
            {
                ee::Renderer::addDrawable(&m_airToCornea);
                ee::Renderer::addDrawable(&m_corneaToLens);
                ee::Renderer::addDrawable(&m_inLens);
                ee::Renderer::addDrawable(&m_lensToCornea);
            }

            DrawLensRayPath(std::string textPack) :
                m_airToCornea(textPack, glm::vec3(), glm::vec3()),
                m_corneaToLens(textPack, glm::vec3(), glm::vec3()),
                m_inLens(textPack, glm::vec3(), glm::vec3()),
                m_lensToCornea(textPack, glm::vec3(), glm::vec3()) {}
        };

        struct LensRayPath
        {
            Line m_airToCornea;
            Line m_corneaToLens;
            Line m_inLens;
            Line m_lensToCornea;
        };

        std::pair<Float, Vec3> intersectCorneaSphere(Ray ray, Float min_dist) const
        {
            Float max_dist = std::numeric_limits<Float>::max();

            Vec3 inv_dir = Vec3(m_invCorneaSphere * Vec4(ray.m_dir, 0.0)); // the direction is the one that is provided
            Vec3 inv_origin = Vec3(m_invCorneaSphere * Vec4(ray.m_origin, 1.0)); // the origin is at the position of the sphere

            Float A = dot(inv_dir, inv_dir);
            Float B = dot(inv_origin, inv_dir);
            Float C = dot(inv_origin, inv_origin) - 1; // unit sphere

            Float discriminant = B * B - A * C;

            // check for the number of solutions:
            if (discriminant < 0)
                return std::make_pair(-1, Vec3());

            Float t_result;
            // two solutions:
            // --------------------------
            if (discriminant > 0)
            {
                Float sqrt_discr = sqrtf(discriminant);

                // from Physically Based Rendering: Theory to Implementation 2nd Ed.
                Float q = -B - glm::sign(B) * sqrt_discr;

                Float t_h1 = q / A;
                Float t_h2 = C / q;

                t_result = glm::min(t_h1, t_h2);
                if (t_result < min_dist) // if this one doesn't work, try the other one
                    t_result = glm::max(t_h1, t_h2);
            }
            // one solution:
            // --------------------------
            else
            {
                t_result = -B / A;
            }

            // check result:
            // --------------------------
            if (t_result > min_dist && t_result < max_dist)
            {
                Vec4 normal = Vec4(inv_origin + (t_result * inv_dir), 0.f);

                normal = glm::transpose(m_invCorneaSphere) * normal;
                normal.w = 0;
                normal = normalize(normal); // normalize the normal

                return std::make_pair(t_result, Vec3(normal));
            }
            else
            {
                return std::make_pair(-1, Vec3());
            }
        }

    private:
        // for debugging:
        // DrawLine* testNormals[13];

        std::vector<DrawLine*> testNormals;

        RayTracer(std::vector<Vec3> positions, Lens sphere, RayTracerParam param);

        Vec3       raytrace(int pos, Float rInc, Float cInc);
        bool       lensRefract(Ray startRay, LensRayPath* o_rayPath, unsigned id); // TODO: remove "debug" ray id from the execution
        Vec3       getNormal(int triangle, Vec3 interPoint, unsigned id);

        const RayTracerParam  m_parameters;

        Lens                  m_lens;

        Mat4                  m_corneaSphere;
        Mat4                  m_invCorneaSphere;

        // intersection caches:
        std::vector<Vec3> m_rayOrigins;
        std::vector<Vec3> m_cachedPoints;

        std::vector<Vec3>            m_resultColors; // the colors that will result
        std::vector<DrawLensRayPath> m_drawableLines; // for rendering (these are all the rays to draw)
    };
}