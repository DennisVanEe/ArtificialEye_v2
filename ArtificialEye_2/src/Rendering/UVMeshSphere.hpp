#pragma once

#include "../SoftBody/Simulation/SBSimulation.hpp"
#include "../SoftBody/Constraints/SBPointConstraint.hpp"
#include "Modeling/Mesh.hpp"

namespace ee
{
    class UVMeshSphere
    {
    public:
        UVMeshSphere(Mesh* mesh, int nLat, int nLon);

        std::vector<ee::SBPointConstraint*> addConstraints(int thickness, ee::SBSimulation* sim);

        Mesh*       getMesh();
        const Mesh* getMesh() const;

        int getNumLatitudes() const;
        int getNumLongitudes() const;

        int getLatitudeIndex(int index) const;
        int getLongitudeIndex(int index) const;

        // makes sure the normals are facing out:
        glm::vec3 getNormal(int faceID) const;

        const std::vector<int>& getLatitudes(int index) const;
        const std::vector<int>& getLongitudes(int index) const;

        int getConstraintStart() const;
        int getConstraintEnd() const;

    private:
        std::vector<std::vector<int>> m_latitudes;
        std::vector<std::vector<int>> m_longitudes;

        Mesh* const m_mesh;
        const int   m_nLatitudes;
        const int   m_nLongitudes;

        int m_constraintStart;
        int m_constraintEnd;
    };
}