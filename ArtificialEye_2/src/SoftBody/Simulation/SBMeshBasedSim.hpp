#pragma once

#include "SBSimulation.hpp"
#include "../Objects/SBVertex.hpp"
#include "../../Rendering/Modeling/DynamicMesh.hpp"

#include <vector>

namespace ee
{
    class SBMeshBasedSim : public SBSimulation
    {
    public:
        SBMeshBasedSim(DynamicMesh* model, float mass, float structStiffness, float structDampening);

        virtual void update(float timeStep) override;

        void addCustomLengthConstraint(float length, std::size_t vertexID0, std::size_t vertexID1);

    protected:
        DynamicMesh* m_model;

        void createSimVertices(float mass);
        void connectSprings(float structStiffness, float structDampening);
    };
}