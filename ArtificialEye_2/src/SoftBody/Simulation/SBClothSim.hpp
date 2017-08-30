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
        SBClothSim(DynamicMesh* model, Float mass, Float structStiffness, Float structDampening);

        virtual void update(Float timeStep) override;

        SBObject* getVertexObject(std::size_t VertexID);

    protected:
        DynamicMesh* m_model;

        void createSimVertices(Float mass);
        void connectSprings(Float structStiffness, Float structDampening);
    };
}