#pragma once

#include <array>
#include "../Types.hpp"

namespace ee
{
    enum class FRAMES_BUFFER_MODE {FLOAT3, FLOAT1, BYTE3, BYTE1};

    class FramesBuffer
    {
    public:
        FramesBuffer() : m_buffer(nullptr) {}
        FramesBuffer(int resolution, int nFrames, FRAMES_BUFFER_MODE mode);
        ~FramesBuffer();

        void initialize(int resolution, int nFrames, FRAMES_BUFFER_MODE mode);

        // This will fail if a maxFrame was set and it has been reached.
        inline bool setPixel(int frame, int framePos, const void* color);

        inline int getNumFrames() const;
        inline int getFrameResolution() const;
        inline int getBufferSize() const;
        inline FRAMES_BUFFER_MODE getMode() const;

        template<class T = Byte>
        const T* getFrame(int frame) const;

        template<class T = Byte>
        const T* getAllFrames() const;

    private:
        FRAMES_BUFFER_MODE m_mode;
        int m_pixelSize;
        int m_resolution;
        int m_frameSize;
        int m_frameCount;

        Byte* m_buffer;
    };
}

int ee::FramesBuffer::getNumFrames() const
{
    return m_frameCount;
}

int ee::FramesBuffer::getFrameResolution() const
{
    return m_resolution;
}

int ee::FramesBuffer::getBufferSize() const
{
    return m_frameSize * m_frameCount;
}

ee::FRAMES_BUFFER_MODE ee::FramesBuffer::getMode() const
{
    return m_mode;
}

template<class T>
const T* ee::FramesBuffer::FramesBuffer::getFrame(int frame) const
{
    const int startOfFrame = frame * m_imageSize;
    return reinterpret_cast<T*>(&m_buffer[startOfFrame]);
}

template<class T>
const T* ee::FramesBuffer::FramesBuffer::getAllFrames() const
{
    return reinterpret_cast<T*>(m_buffer);
}

bool ee::FramesBuffer::setPixel(int frame, int framePos, const void* color)
{
    if (frame >= m_frameCount || framePos > m_resolution)
    {
        return false;
    }

    const int startOfFrame = frame * m_frameCount;
    const int totalOffset = startOfFrame + framePos;

    std::memcpy(&m_buffer[totalOffset], color, m_pixelSize);

    return true;
}