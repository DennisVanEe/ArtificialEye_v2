#pragma once

#include "SBMeshBasedSim.hpp"
#include "../../SoftBody/ForceGens/SBLocalForceGen.hpp"

namespace ee
{
    class SBClosedBodySim : public SBMeshBasedSim
    {
    public:
        SBClosedBodySim(float P, DynamicMesh* model, float mass, float stiffness, float dampening);

        void setP(float P);

    private:
        friend class SBPressure;

        // This is designed for closed body meshes, so, technically
        // this does not include the regular pressure forces
        // Really, it will only be used by Cloth Sim.
        class SBPressure : public SBLocalForceGen
        {
        public:
            // some gaurantees
            SBPressure(float P, DynamicMesh* model, SBMeshBasedSim* simulation);

            void applyForces() override;
            SBLocalForceGen* getCopy() const override;

        public:
            float m_P;

        private:
            DynamicMesh*    const m_model;
            SBMeshBasedSim* const m_simulation;
        }* m_pressure;
    };
}