#pragma once

#include <vector>
#include <memory>

#include "../ForceGens/SBForceGen.hpp"
#include "../Integrators/SBIntegrator.hpp"
#include "../Objects/SBObject.hpp"
#include "../SBSpring.hpp"
#include "../Constraints/SBConstraint.hpp"

namespace ee
{
    // This is to help make it easier to describe the objects

    using SBObjectList = std::vector<std::unique_ptr<SBObject>>;
    using SBForceGenList = std::vector<std::unique_ptr<SBForceGen>>;
    using SBSpringList = std::vector<std::unique_ptr<SBSpring>>;
    using SBConstraintList = std::vector<std::unique_ptr<SBConstraint>>;

    class SBSimulation
    {
    public:
        void addSpring(Float stiffness, Float dampening, SBObject* objA, SBObject* objB);
        void addSpring(Float stiffness, Float dampening, Float length, SBObject* objA, SBObject* objB);
        void addObject(SBObject* obj);
        void addGlobalForceGen(SBForceGen* force);
        void addConstraint(SBConstraint* constraint);
        void addIntegrator(SBIntegrator* integrator);

        virtual void update(Float timeStep);

    public:
        std::size_t m_constIterations;

    protected:
        SBObjectList m_objects;
        SBForceGenList m_globalForceGens;
        SBSpringList m_springs; // so that we have the same interface
        std::unique_ptr<SBIntegrator> m_integrator;

        SBConstraintList m_constraints;
    };
}