#pragma once

#include "SBMeshBasedSim.hpp"
#include "../../SoftBody/ForceGens/SBLocalForceGen.hpp"

namespace ee
{
    class SBClosedBodySim : public SBMeshBasedSim
    {
    public:
        SBClosedBodySim(Float P, Mesh* model, Float mass, Float stiffness, Float dampening);

        void setP(Float P);

    private:
        friend class SBPressure;

        // This is designed for closed body meshes, so, technically
        // this does not include the regular pressure forces
        // Really, it will only be used by Cloth Sim.
        class SBPressure : public SBLocalForceGen
        {
        public:
            // some gaurantees
            SBPressure(Float P, Mesh* model, SBMeshBasedSim* simulation);

            void applyForces() override;
            SBLocalForceGen* getCopy() const override;

        public:
            Float m_P;

        private:
            Mesh*           const m_model;
            SBMeshBasedSim* const m_simulation;
        }* m_pressure;
    };
}