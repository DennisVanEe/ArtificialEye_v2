#pragma once

#include "../SoftBody/Simulation/SBSimulation.hpp"
#include "../SoftBody/Constraints/SBPointConstraint.hpp"
#include "Modeling/Mesh.hpp"

namespace ee
{
    class UVMeshSphere
    {
    public:
        UVMeshSphere(Mesh* mesh, unsigned nLat, unsigned nLon);

        std::vector<ee::SBPointConstraint*> addConstraints(std::size_t thickness, ee::SBSimulation* sim);

        Mesh*       getMesh();
        const Mesh* getMesh() const;

        unsigned getNumLatitudes() const;
        unsigned getNumLongitudes() const;

        unsigned getLatitudeIndex(std::size_t index) const;
        unsigned getLongitudeIndex(std::size_t index) const;

        const std::vector<std::size_t>& getLatitudes(unsigned index) const;
        const std::vector<std::size_t>& getLongitudes(unsigned index) const;

        unsigned getMuscleEnd() const;
        unsigned getMuscleBegin() const;

    private:
        std::vector<std::vector<std::size_t>> m_latitudes;
        std::vector<std::vector<std::size_t>> m_longitudes;

        Mesh* const m_mesh;
        const unsigned m_nLatitudes;
        const unsigned m_nLongitudes;

        unsigned m_muscleBegin;
        unsigned m_muslceEnd;
    };
}