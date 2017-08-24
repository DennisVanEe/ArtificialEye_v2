#pragma once

#include "SBSimulation.hpp"
#include "../Objects/SBVertex.hpp"
#include "../../Rendering/DynamicModel.hpp"

#include <vector>

namespace ee
{
    class SBClothSim : public SBSimulation
    {
    public:
        SBClothSim(DynamicModel* model, Float mass, Float structStiffness, Float structDampening, Float shearStiffness,
            Float shearDampening, Float bendStiffness, Float bendDampening);

        virtual void update(Float timeStep) override;

        SBObject* getVertexObject(std::size_t VertexID);

    protected:
        DynamicModel* m_model;

        void createSimVertices(Float mass);
        void connectSprings(Float structStiffness, Float structDampening, Float shearStiffness, Float shearDampening,
            Float bendStiffness, Float bendDampening);
    };
}