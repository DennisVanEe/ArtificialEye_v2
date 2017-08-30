#include "SBModel.hpp"

ee::SBModel::SBModel(Mesh model, Float mass, SBObjectType type) :
    SBObject(mass, type),
    m_model(model)
{
}

void ee::SBModel::update(Float time)
{
}
