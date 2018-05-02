#pragma once

#include "Types.hpp"
#include <string>

// Global Constants:
const std::string ARTIFICIAL_EYE_PROP_DIR = "ArtificialEye_Properties.ini";

namespace ee
{
	// All the properties that the lens needs
    struct ArtificialEyeProp
    {
        bool            success; // true if loading succeeds

        int             threads;

        std::string     input_file;
        std::string     output_file;

        float           distance_width;
        float           distance_height;
        int             photo_res_width;
        int             photo_res_height;
        int             samples;

        int			    latitude;
		int				longitude;
		int			    iterations;

        float           mass;
        float           intspring_coeff;
        float           intspring_drag;
        float           extspring_coeff;
        float           extspring_drag;

        float           pressure;
        int             muscle_thickness;
        float           lens_refr_index;
        float           lens_thickness;

        int             subdiv_level_lens;
		float			lens_dist_from_center;

		float			eyeball_radius;
		float			eyeball_refr_index;
		float			time_step;

		std::string		shader_dir;
    };

    ArtificialEyeProp initializeArtificialEyeProp(const std::string& dir);

	extern ArtificialEyeProp ARTIFICIAL_EYE_PROP;
}