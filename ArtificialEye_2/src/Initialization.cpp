#include "Initialization.hpp"

#include <windows.h>
#include <sstream>
#include <iostream>

// helper functions

char g_writeBuffer[1024];

float getFloat(const std::string& section, const std::string& name, const std::string& file)
{
    DWORD size = GetPrivateProfileString(section.c_str(), name.c_str(), nullptr, g_writeBuffer, 1024, file.c_str()); // does not include null char
    if (size == 0)
    {
        std::stringstream str;
        str << "Could not find " << name << " under " << section << " from .ini file at: " << file << ".";
        throw std::runtime_error(str.str());
    }
    
    float value;
    try
    {
        value = std::stof(g_writeBuffer, nullptr);
    }
    catch (...)
    {
        std::stringstream str;
        str << "Could not convert " << name << " under " << section << " from .ini file at: " << file << " to float.";
        throw std::runtime_error(str.str());
    }
    return value;
}

std::string getStr(const std::string& section, const std::string& name, const std::string& file)
{
    DWORD size = GetPrivateProfileString(section.c_str(), name.c_str(), nullptr, g_writeBuffer, 1024, file.c_str()); // does not include null char
    if (size == 0)
    {
        std::stringstream str;
        str << "Could not find string value at " << name << " under " << section << " from .ini file at: " << file << ".";
        throw std::runtime_error(str.str());
    }

    return std::string(g_writeBuffer);
}

std::size_t getUInt(const std::string& section, const std::string& name, const std::string& file)
{
    UINT result = GetPrivateProfileInt(section.c_str(), name.c_str(), -1, file.c_str());
    if (result < 0)
    {
        std::stringstream str;
        str << "Could not find unsigned value at " << name << " under " << section << " from .ini file at: " << file << ".";
        throw std::runtime_error(str.str());
    }
    return result;
}

ee::ArtificialEyeProp ee::initializeArtificialEyeProp(const std::string& dir)
{
    // This is the windows specific implementation (too lazy to get another library)

    ArtificialEyeProp result;

    try
    {
        result.shader_dir =                     getStr  ("graphics", "shader_dir",       dir);
        result.render_param.m_screenWidth =     getUInt ("graphics", "screen_width",     dir);
        result.render_param.m_screenHeight =    getUInt ("graphics", "screen_height",    dir);
        result.render_param.m_fov =             getFloat("graphics", "fov",              dir);
        result.render_param.m_far =             getFloat("graphics", "far_plane",        dir);
        result.render_param.m_near =            getFloat("graphics", "near_plane",       dir);
        result.render_param.m_aspect = result.render_param.m_screenWidth / result.render_param.m_screenHeight;

        result.latitude =                       getUInt ("lens",     "latitude",         dir);
        result.longitude =                      getUInt ("lens",     "longitude",        dir);
        result.iterations =                     getUInt ("lens",     "iterations",       dir);
        result.mass =                           getFloat("lens",     "mass",             dir);
        result.intspring_coeff =                getFloat("lens",     "intspring_coeff",  dir);
        result.intspring_drag =                 getFloat("lens",     "intspring_drag",   dir);
        result.extspring_coeff =                getFloat("lens",     "extspring_coeff",  dir);
        result.extspring_drag =                 getFloat("lens",     "extspring_drag",   dir);
        result.pressure =                       getFloat("lens",     "pressure",         dir);
        result.muscle_thickness =               getFloat("lense",    "muslce_thickness", dir);
    }
    catch (const std::exception& e)
    {
        std::cout << "[EXCEP THROWN]: " << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "Press ENTER to exit." << std::endl;
        std::cin.get();
        result.success = false;
    }

    result.success = true;
    return result;
}