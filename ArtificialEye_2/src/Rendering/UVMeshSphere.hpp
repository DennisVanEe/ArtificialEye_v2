#pragma once

#include "../SoftBody/Simulation/SBSimulation.hpp"
#include "../SoftBody/Constraints/SBPointConstraint.hpp"
#include "Modeling/Mesh.hpp"

namespace ee
{
    class UVMeshSphere
    {
    public:
        UVMeshSphere(Mesh* mesh, unsigned lat, unsigned lon);

        std::vector<ee::SBPointConstraint*> addConstraints(const std::size_t thickness, ee::SBSimulation* sim);

        Mesh* getMesh();
        const Mesh* getMesh() const;

        unsigned getLatitudes() const;
        unsigned getLongitudes() const;

        unsigned getMuscleEnd() const;
        unsigned getMuscleBegin() const;

    private:
        Mesh* const m_mesh;
        const unsigned m_latitudes;
        const unsigned m_longitudes;

        unsigned m_muscleBegin;
        unsigned m_muslceEnd;
    };
}