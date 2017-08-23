#include "SBModel.hpp"

ee::SBModel::SBModel(Model model, Float mass, SBObjectType type) :
    SBObject(mass, type),
    m_model(model)
{
}

void ee::SBModel::update(Float time)
{
}
