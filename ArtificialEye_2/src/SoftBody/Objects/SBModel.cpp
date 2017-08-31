#include "SBModel.hpp"

ee::SBModel::SBModel(Mesh model, float mass, SBObjectType type) :
    SBObject(mass, type),
    m_model(model)
{
}

void ee::SBModel::update(float time)
{
}
