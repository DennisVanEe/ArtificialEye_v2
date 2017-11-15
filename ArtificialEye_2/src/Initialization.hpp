#pragma once

#include "Rendering/Renderer.hpp"

#include <string>

// Global Constants:
const std::string ARTIFICIAL_EYE_PROP_DIR = "ArtificialEye_Properties.ini";

namespace ee
{
    struct ArtificialEyeProp
    {
        bool            success;

        std::string     shader_dir;
        RendererParam   render_param;

        std::size_t     latitude;
        std::size_t     longitude;
        std::size_t     iterations;

        Float           mass;
        Float           intspring_coeff;
        Float           intspring_drag;
        Float           extspring_coeff;
        Float           extspring_drag;

        Float           pressure;
        unsigned        muscle_thickness;
        Float           refractive_index;
        Float           lens_thickness;

        unsigned        subdiv_level_lens;
        unsigned        subdiv_level_cornea;
    };

    ArtificialEyeProp initializeArtificialEyeProp(const std::string& dir);

    extern const ArtificialEyeProp ARTIFICIAL_EYE_PROP;
}