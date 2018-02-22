#include "Initialization.hpp"

#include <INIReader.hpp>
#include <sstream>
#include <iostream>

// helper functions

float F(double f)
{
	return static_cast<float>(f);
}

template<class T>
void errorLessThanZero(T val, bool* success)
{
	T zero = static_cast<T>(0);
	*success = val >= zero;
}

ee::ArtificialEyeProp ee::initializeArtificialEyeProp(const std::string& dir)
{
    // This is the windows specific implementation (too lazy to get another library)

    ArtificialEyeProp result;
    result.success = true;

    try
    {
		INIReader reader(dir);
        if (reader.ParseError() < 0)
        {
            throw std::runtime_error("Could not open file " + dir);
        }

        result.latitude = reader.GetInteger("lens", "latitude", -1);
		errorLessThanZero(result.latitude, &result.success);

        result.longitude = reader.GetInteger("lens", "longitude", -1);
		errorLessThanZero(result.longitude, &result.success);

        result.iterations =	reader.GetInteger("lens", "iterations", -1);
		errorLessThanZero(result.iterations, &result.success);

		result.mass = F(reader.GetReal("lens", "mass", -1.0));
		errorLessThanZero(result.mass, &result.success);

		result.intspring_coeff = F(reader.GetReal("lens", "intspring_coeff", -1.0));
		errorLessThanZero(result.intspring_coeff, &result.success);

		result.intspring_drag = F(reader.GetReal("lens", "intspring_drag", -1.0));
		errorLessThanZero(result.intspring_drag, &result.success);

		result.extspring_coeff = F(reader.GetReal("lens", "extspring_coeff", -1.0));
		errorLessThanZero(result.extspring_coeff, &result.success);

		result.extspring_drag = F(reader.GetReal("lens", "extspring_drag", -1.0));
		errorLessThanZero(result.extspring_drag, &result.success);

		result.pressure = F(reader.GetReal("lens", "pressure", -1.0));
		errorLessThanZero(result.pressure, &result.success);

        result.muscle_thickness = reader.GetInteger ("lens", "muscle_thickness", -1);
		errorLessThanZero(result.muscle_thickness, &result.success);

		result.lens_refr_index = F(reader.GetReal("lens", "refractive_index", -1.0));
		errorLessThanZero(result.lens_refr_index, &result.success);

		result.lens_thickness = F(reader.GetReal("lens", "lens_thickness", -1.0));
		errorLessThanZero(result.lens_thickness, &result.success);

        result.subdiv_level_lens = reader.GetInteger("lens", "subdiv_level", -1);
		errorLessThanZero(result.subdiv_level_lens, &result.success);

		result.lens_dist_from_center = F(reader.GetReal("lens", "distance_from_center", -1.0));
		errorLessThanZero(result.lens_dist_from_center, &result.success);


		result.eyeball_radius = F(reader.GetReal("eyeball", "radius", -1.0));
		errorLessThanZero(result.eyeball_radius, &result.success);

		result.eyeball_refr_index = F(reader.GetReal("eyeball", "refractive_index", -1.0));
		errorLessThanZero(result.eyeball_refr_index, &result.success);


		result.time_step = F(reader.GetReal("simulation", "time_step", -1.0));
		errorLessThanZero(result.time_step, &result.success);

		result.shader_dir = reader.Get("graphics", "shader_dir", "");
		result.success = result.shader_dir != "";
    }
    catch (const std::exception& e)
    {
        std::cout << "[EXCEP THROWN]: " << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "Press ENTER to exit." << std::endl;
        std::cin.get();

        result.success = false;
    }

    return result;
}

ee::ArtificialEyeProp ee::ARTIFICIAL_EYE_PROP = ee::initializeArtificialEyeProp("prop.ini");