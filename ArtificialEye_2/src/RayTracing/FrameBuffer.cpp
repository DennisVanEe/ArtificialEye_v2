#include "FrameBuffer.hpp"

ee::FramesBuffer::FramesBuffer(int resolution, int nFrames, FRAMES_BUFFER_MODE mode) :
    m_mode(mode),
    m_resolution(resolution),
    m_frameCount(nFrames)
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

    m_frameSize = m_pixelSize * resolution;
    m_buffer = new Byte[m_frameSize * m_frameCount];
}

ee::FramesBuffer::~FramesBuffer()
{
    delete[] m_buffer;
}