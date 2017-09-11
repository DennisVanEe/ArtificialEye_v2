#pragma once

#include "Simulation/SBClosedBodySim.hpp"

namespace ee
{
    void addBendSpringsForUVSphere(SBClosedBodySim* sim, unsigned nLat, unsigned nLon, float stiffness, float dampening);
}