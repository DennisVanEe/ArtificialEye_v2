#include "camera.hpp"
#pragma once

// Default camera values
const float YAW         = -90.f;
const float PITCH       =   0.f;
const float SPEED       =  2.5f;
const float SENSITIVTY  =  0.1f;
const float ZOOM        =  45.f;

ee::Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch) :
	m_pos(pos),
	m_worldUp(up),
	m_yaw(yaw),
	m_pitch(pitch),
	m_msensitivity(0.1f),
	m_speed(2.5f),
	m_zoom(45.f),
	m_front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	updateCam();
}

glm::mat4 ee::Camera::viewMatrix() const
{
    // use yaw and pitch for direction instead of a target position 
    // this is easier for mouse controlls then.
    return glm::lookAt(m_pos, m_pos + m_front, m_up);
}

void ee::Camera::processKBInput(CameraDir direction, float deltaTime)
{
    float deltaPos = m_speed * deltaTime;
	switch (direction)
	{
    case CameraDir::FORWARD:
		m_pos += m_front * deltaPos;
		break;
    case CameraDir::BACKWARD:
		m_pos -= m_front * deltaPos;
		break;
    case CameraDir::LEFT:
		m_pos -= m_right * deltaPos;
		break;
    case CameraDir::RIGHT:
		m_pos += m_right * deltaPos;
		break;
	}
}

void ee::Camera::processMInput(float xOffset, float yOffset, GLboolean constrainPitch)
{
	xOffset *= m_msensitivity;
	yOffset *= m_msensitivity;

	m_yaw += xOffset;
	m_pitch += yOffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
        if (m_pitch > 89.f)
        {
            m_pitch = 89.f;
        }
        if (m_pitch < -89.f)
        {
            m_pitch = -89.f;
        }
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	updateCam();
}

void ee::Camera::processSInput(float scrOffset)
{
    if (m_zoom >= 1.f && m_zoom <= 45.f)
    {
        m_zoom -= scrOffset;
    }
    if (m_zoom <= 1.f)
    {
        m_zoom = 1.f;
    }
    if (m_zoom >= 45.f)
    {
        m_zoom = 45.f;
    }
}

glm::vec3 ee::Camera::getPosition() const
{
    return m_pos;
}

void ee::Camera::updateCam()
{
	// Calculate the new Front vector
    glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_up = glm::normalize(glm::cross(m_right, m_front));
}