#include "SBUtilities.hpp"

void ee::addInteriorSpringsUVSphere(SBClosedBodySim* const sim, const unsigned nLat, const unsigned nLon, const float stiffness, const float dampening)
{
    // This is important, this is only gauranteed to work flawlessly with UV spheres, any other closed body sim is not gauranteed to work.
    // Going in rings is incremental, so adding those is easy

    // add the constraints to the caps:
    sim->addSpring(stiffness, dampening, sim->getVertexObject(0), sim->getVertexObject(sim->getNumVertexObjects() - 1));
    float length = glm::length(sim->getVertexObject(0)->m_currPosition - sim->getVertexObject(sim->getNumVertexObjects() - 1)->m_currPosition);
    //sim->addConstraint(new SBLengthConstraint(length, sim->getVertexObject(0), sim->getVertexObject(sim->getNumVertexObjects() - 1)));

    for (std::size_t i = 0; i < nLat / 2; i++)
    {
        for (std::size_t j = 1; j <= nLon; j++)
        {
            std::size_t index0 = j + (i * nLon);
            std::size_t index1 = j + ((nLat - 1 - i) * nLon);

            sim->addSpring(stiffness, dampening, sim->getVertexObject(index0), sim->getVertexObject(index1));
            float length = glm::length(sim->getVertexObject(index0)->m_currPosition - sim->getVertexObject(index1)->m_currPosition);
            sim->addConstraint(new SBLengthConstraint(length, sim->getVertexObject(index0), sim->getVertexObject(index1), 0.9f));
        }
    }
}