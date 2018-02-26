#include "PlaneBuffer.hpp"

#include "glm/gtc/type_ptr.hpp"

#include <iostream>

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

bool ee::ImageBuffer::setPixel(int x, int y, glm::u8vec3 color)
{
	if (x >= m_width || y >= m_height)
	{
		return false; // this would obviously never work
	}

	int index = (y * m_width + x);

	m_imageBuffer[index] = color;

	return true;
}

unsigned ee::ImageBuffer::getTextureID() const
{
	unsigned texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_imageBuffer.data());

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}