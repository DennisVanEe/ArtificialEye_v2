#pragma once

#include <glm/common.hpp>
#include <glad/glad.h>
#include <vector>
#include "..\RayTracing\ImageBuffer.hpp"

namespace ee
{
	class ImageBuffer
	{
	public:
		ImageBuffer(int width, int height);
		~ImageBuffer();

		inline bool setPixel(int x, int y, float color); // change an individual pixel color

		unsigned getTextureID() const;

        int getWidth() const
		{
			return m_width;
		}

		int getHeight() const
		{
			return m_height;
		}

	private:
		GLuint m_texture;
		std::vector<glm::u8vec3> m_imageBuffer;
		int m_width, m_height;
	};
}