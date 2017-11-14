#pragma once

#include "SBSimulation.hpp"
#include "../Objects/SBVertex.hpp"
#include "../../Rendering/Modeling/Mesh.hpp"

#include <vector>

namespace ee
{
    class SBMeshBasedSim : public SBSimulation
    {
    public:
        SBMeshBasedSim(Mesh* model, Float mass, Float structStiffness, Float structDampening);

        virtual void update(Float timeStep) override;

        void addCustomLengthConstraint(Float length, std::size_t vertexID0, std::size_t vertexID1);

    protected:
        Mesh* m_model;

        void createSimVertices(Float mass);
        void connectSprings(Float structStiffness, Float structDampening);
    };
}