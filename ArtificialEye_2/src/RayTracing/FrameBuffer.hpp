#pragma once

#include <array>
#include "../Types.hpp"

namespace ee
{
    enum class FRAMES_BUFFER_MODE {FLOAT3, FLOAT1, BYTE3, BYTE1};

    class FramesBuffer
    {
    public:
        FramesBuffer(int width, int height, int maxFrames, FRAMES_BUFFER_MODE mode);
        ~FramesBuffer();

        // This will fail if a maxFrame was set and it has been reached.
        inline bool setPixel(int frame, int width, int height, const void* color);

        inline int getMaxNumFrames() const;
        inline int getWidth() const;
        inline int getHeight() const;

        int getBufferSize() const;

        template<class T = Byte>
        const T* getBuffer() const;

    private:
        FRAMES_BUFFER_MODE m_mode;
        int m_pixelSize;
        int m_imageSize;
        int m_maxFrames;
        int m_width, m_height;

        Byte* m_buffer;
    };
}

int ee::FramesBuffer::getMaxNumFrames() const
{
    return m_maxFrames;
}

int ee::FramesBuffer::getWidth() const
{
    return m_width;
}

int ee::FramesBuffer::getHeight() const
{
    return m_height;
}

int ee::FramesBuffer::getBufferSize() const
{
    return m_imageSize * m_maxFrames;
}

template<class T>
const T* ee::FramesBuffer::FramesBuffer::getBuffer() const
{
    return reinterpret_cast<T*>(m_buffer);
}

bool ee::FramesBuffer::setPixel(int frame, int width, int height, const void* color)
{
    if (frame >= m_maxFrames || width >= m_width || height >= m_height)
    {
        return false;
    }

    const int startOfFrame = frame * m_imageSize;
    const int frameOffset = m_pixelSize * (height * m_width + width);
    const int totalOffset = startOfFrame + frameOffset;

    std::memcpy(&m_buffer[totalOffset], color, m_pixelSize);

    return true;
}