#pragma once

#include "SBSimulation.hpp"
#include "../Objects/SBVertex.hpp"
#include "../../Rendering/Modeling/DynamicMesh.hpp"

#include <vector>

namespace ee
{
    class SBClothSim : public SBSimulation
    {
    public:
        SBClothSim(DynamicMesh* model, float mass, float structStiffness, float structDampening);

        virtual void update(float timeStep) override;

        SBObject* getVertexObject(std::size_t VertexID);

    protected:
        DynamicMesh* m_model;

        void createSimVertices(float mass);
        void connectSprings(float structStiffness, float structDampening);
    };
}