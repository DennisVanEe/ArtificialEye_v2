#include "SBUtilities.hpp"

void ee::addBendSpringsForUVSphere(SBClosedBodySim* const sim, const unsigned nLat, const unsigned nLon, const float stiffness, const float dampening)
{
    // This is important, this is only gauranteed to work flawlessly with UV spheres, any other closed body sim is not gauranteed to work.
    // Going in rings is incremental, so adding those is easy

    // do the longitudinal rings first:
    // we want to skip the first one (and last one)
    for (std::size_t i = 0; i < nLat; i++)
    {
        for (std::size_t j = 1; j < nLon; j++)
        {
            std::size_t index0 = j + (i * nLon);
            std::size_t index1 = ((j + 2) % nLon) + (i * nLon);

            sim->addSpring(stiffness, dampening, sim->getVertexObject(index0), sim->getVertexObject(index1));
        }
    }
}