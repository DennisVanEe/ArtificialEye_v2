#include "FrameBuffer.hpp"

ee::FramesBuffer::FramesBuffer(int width, int height, int maxFrames, FRAMES_BUFFER_MODE mode) :
    m_mode(mode),
    m_maxFrames(maxFrames),
    m_width(width), m_height(height)
{
    switch (mode)
    {
    case FRAMES_BUFFER_MODE::FLOAT3:
        m_pixelSize = 3 * sizeof(float);
        break;
    case FRAMES_BUFFER_MODE::FLOAT1:
        m_pixelSize = sizeof(float);
        break;
    case FRAMES_BUFFER_MODE::BYTE3:
        m_pixelSize = 3;
        break;
    case FRAMES_BUFFER_MODE::BYTE1:
        m_pixelSize = 1;
        break;
    }

    m_imageSize = m_pixelSize * m_width * m_height;
    m_buffer = new Byte[m_imageSize * maxFrames];
}

ee::FramesBuffer::~FramesBuffer()
{
    delete[] m_buffer;
}