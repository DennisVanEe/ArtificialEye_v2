#include "PlaneBuffer.hpp"

#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include "..\RayTracing\ImageBuffer.hpp"

using namespace std;
using namespace glm;

ee::ImageBuffer::ImageBuffer(int width, int height) :
	m_width(width),
	m_height(height),
	m_imageBuffer(width * height)
{
}

ee::ImageBuffer::~ImageBuffer()
{
	std::cout << "ImageBuffer destructor called." << std::endl;
	glDeleteTextures(1, &m_texture);
}

bool ee::ImageBuffer::setPixel(int x, int y, float color)
{
	if (x >= m_width || y >= m_height)
	{
		return false; // this would obviously never work
	}

	int index = (y * m_width + x);

    unsigned char greyscale = 256.f * color;
    m_imageBuffer[index] = glm::u8vec3(greyscale);

	return true;
}

unsigned ee::ImageBuffer::getTextureID() const
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<const unsigned char*>(m_imageBuffer.data()));

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}