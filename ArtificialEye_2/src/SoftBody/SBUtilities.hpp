#pragma once

#include "Simulation/SBClosedBodySim.hpp"
#include "Constraints/SBLengthConstraint.hpp"

namespace ee
{
    void addInteriorSpringsUVSphere(SBClosedBodySim* sim, unsigned nLat, unsigned nLon, float stiffness, float dampening);
}