#include "Initialization.hpp"

#include <fstream>
#include <windows.h>
#include <sstream>
#include <iostream>

// helper functions

char g_writeBuffer[1024];
char g_currDirBuffer[1024];

float getFloat(const std::string& section, const std::string& name, const std::string& file)
{
    DWORD bytes = GetModuleFileName(NULL, g_currDirBuffer, 1024);
    std::string dir(g_currDirBuffer);
    dir = dir.substr(0, dir.find_last_of('\\') + 1);
    DWORD size = GetPrivateProfileString(section.c_str(), name.c_str(), nullptr, g_writeBuffer, 1024, (dir + file).c_str()); // does not include null char
    if (size == 0)
    {
        std::stringstream str;
        str << "Could not find " << name << " under " << section << " from .ini file at: " << file << "." << std::endl;
        str << "The following error was returned (WINAPI): " << GetLastError();
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
        str << "Could not convert " << name << " under " << section << " from .ini file at: " << file << " to float." << std::endl;
        throw std::runtime_error(str.str());
    }
    return value;
}

std::string getStr(const std::string& section, const std::string& name, const std::string& file)
{
    DWORD bytes = GetModuleFileName(NULL, g_currDirBuffer, 1024);
    std::string dir(g_currDirBuffer);
    dir = dir.substr(0, dir.find_last_of('\\') + 1);
    DWORD size = GetPrivateProfileString(section.c_str(), name.c_str(), nullptr, g_writeBuffer, 1024, (dir + file).c_str()); // does not include null char
    if (size == 0)
    {
        std::stringstream str;
        str << "Could not find string value at " << name << " under " << section << " from .ini file at: " << file << "." << std::endl;
        str << "The following error was returned (WINAPI): " << GetLastError();
        throw std::runtime_error(str.str());
    }

    return std::string(g_writeBuffer);
}

std::size_t getUInt(const std::string& section, const std::string& name, const std::string& file)
{
    // outdated API, so I have to do this
    DWORD bytes = GetModuleFileName(NULL, g_currDirBuffer, 1024);
    std::string dir(g_currDirBuffer);
    dir = dir.substr(0, dir.find_last_of('\\') + 1);
    UINT result = GetPrivateProfileInt(section.c_str(), name.c_str(), -1, (dir + file).c_str());
    if (result < 0)
    {
        std::stringstream str;
        str << "Could not find unsigned value at " << name << " under " << section << " from .ini file at: " << file << "." << std::endl;
        str << "The following error was returned (WINAPI): " << GetLastError();
        throw std::runtime_error(str.str());
    }
    return result;
}

ee::ArtificialEyeProp ee::initializeArtificialEyeProp(const std::string& dir)
{
    // This is the windows specific implementation (too lazy to get another library)

    ArtificialEyeProp result;
    result.success = true;

    try
    {
        std::ifstream file(dir);
        if (!file.is_open())
        {
            file.close();
            throw std::runtime_error("Could not open file " + dir);
        }
        file.close();

        result.shader_dir =                     getStr  ("graphics", "shader_dir",       dir);
        result.render_param.m_screenWidth =     getUInt ("graphics", "screen_width",     dir);
        result.render_param.m_screenHeight =    getUInt ("graphics", "screen_height",    dir);
        result.render_param.m_fov =             getFloat("graphics", "fov",              dir);
        result.render_param.m_far =             getFloat("graphics", "far_plane",        dir);
        result.render_param.m_near =            getFloat("graphics", "near_plane",       dir);
        result.render_param.m_aspect = static_cast<float>(result.render_param.m_screenWidth) / result.render_param.m_screenHeight;

        result.latitude =                       getUInt ("lens",     "latitude",         dir);
        result.longitude =                      getUInt ("lens",     "longitude",        dir);
        result.iterations =                     getUInt ("lens",     "iterations",       dir);
        result.mass =                           getFloat("lens",     "mass",             dir);
        result.intspring_coeff =                getFloat("lens",     "intspring_coeff",  dir);
        result.intspring_drag =                 getFloat("lens",     "intspring_drag",   dir);
        result.extspring_coeff =                getFloat("lens",     "extspring_coeff",  dir);
        result.extspring_drag =                 getFloat("lens",     "extspring_drag",   dir);
        result.pressure =                       getFloat("lens",     "pressure",         dir);
        result.muscle_thickness =               getUInt ("lens",     "muscle_thickness", dir);
        result.refractive_index =               getFloat("lens",     "refractive_index", dir);
        result.lens_thickness =                 getFloat("lens",     "lens_thickness",   dir);
        result.subdiv_level =                   getUInt ("lens",     "subdiv_level",     dir);
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

const ee::ArtificialEyeProp ee::ARTIFICIAL_EYE_PROP = ee::initializeArtificialEyeProp(ARTIFICIAL_EYE_PROP_DIR);