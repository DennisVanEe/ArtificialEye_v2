#pragma once

#include <vector>
#include <memory>

#include "../ForceGens/SBGlobalForceGen.hpp"
#include "../ForceGens/SBLocalForceGen.hpp"
#include "../Integrators/SBIntegrator.hpp"
#include "../Objects/SBObject.hpp"
#include "../SBSpring.hpp"
#include "../Constraints/SBConstraint.hpp"

namespace ee
{
    // This is to help make it easier to describe the objects

    using SBObjectList = std::vector<std::unique_ptr<SBObject>>;
    using SBGlobalForceGenList = std::vector<std::unique_ptr<SBGlobalForceGen>>;
    using SBLocalForceGenList = std::vector<std::unique_ptr<SBLocalForceGen>>;
    using SBSpringList = std::vector<std::unique_ptr<SBSpring>>;
    using SBConstraintList = std::vector<std::unique_ptr<SBConstraint>>;

    class SBSimulation
    {
    public:
        void addSpring(Float stiffness, Float dampening, SBObject* objA, SBObject* objB);
        void addSpring(Float stiffness, Float dampening, Float length, SBObject* objA, SBObject* objB);
        void addObject(SBObject* obj);
        void addGlobalForceGen(SBGlobalForceGen* force);
        void addLocalForceGen(SBLocalForceGen* force);
        void addConstraint(SBConstraint* constraint);
        void addIntegrator(SBIntegrator* integrator);

        virtual void update(Float timeStep);

    public:
        std::size_t m_constIterations;

    protected:
        SBObjectList m_objects;
        SBGlobalForceGenList m_globalForceGens;
        SBLocalForceGenList m_localForceGens;
        SBSpringList m_springs; // so that we have the same interface
        std::unique_ptr<SBIntegrator> m_integrator;

        SBConstraintList m_constraints;
    };
}