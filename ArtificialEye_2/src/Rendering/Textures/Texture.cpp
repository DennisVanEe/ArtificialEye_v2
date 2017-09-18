#include "Texture.hpp"

ee::Texture::Texture(Texture&& other) :
    m_texture(other.m_texture)
{
    m_texture = 0;
}
