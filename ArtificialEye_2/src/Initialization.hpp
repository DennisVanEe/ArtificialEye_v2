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

        float           mass;
        float           intspring_coeff;
        float           intspring_drag;
        float           extspring_coeff;
        float           extspring_drag;

        float           pressure;
        unsigned        muscle_thickness;
        float           refractive_index;
        float           lens_thickness;
    };

    ArtificialEyeProp initializeArtificialEyeProp(const std::string& dir);

    const ArtificialEyeProp ARTIFICIAL_EYE_PROP = initializeArtificialEyeProp(ARTIFICIAL_EYE_PROP_DIR);
}