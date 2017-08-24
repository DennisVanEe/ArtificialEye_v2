#pragma once

#include "SBClothSim.hpp"
#include "../../SoftBody/ForceGens/SBLocalForceGen.hpp"

namespace ee
{
    class SBClosedBody : public SBClothSim
    {
    public:
        SBClosedBody(Float P, DynamicModel* model, Float mass, Float structStiffness, Float structDampening, Float shearStiffness,
            Float shearDampening, Float bendStiffness, Float bendDampening);

    private:
        friend class SBPressure;

        // This is designed for closed body meshes, so, technically
        // this does not include the regular pressure forces
        // Really, it will only be used by Cloth Sim.
        class SBPressure : public SBLocalForceGen
        {
        public:
            // some gaurantees
            SBPressure(Float P, DynamicModel* model, SBClosedBody* simulation);

            void applyForces() override;
            SBLocalForceGen* getCopy() const override;

        public:
            DynamicModel* const m_model;
            SBClosedBody* const m_simulation;
            Float m_P;
        };
    };
}