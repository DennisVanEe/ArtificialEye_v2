#pragma once

#include <glad/glad.h>
#include <vector>

#include "../Types.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace ee
{
	enum WorldDirection
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	class Camera
	{
	public:
		Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch);

		glm::mat4 viewMatrix() const
		{
			// use yaw and pitch for direction instead of a target position 
			// this is easier for mouse controlls then.
			return glm::lookAt(m_pos, m_pos + m_front, m_up);
		}

		void processKBInput(WorldDirection direction, float deltaTime);

		void processMInput(float xOffset, float yOffset, GLboolean constrainPitch = true);
		void processSInput(float scrOffset);

        Vector3 getPosition() const;

	private:

		// Camera Attributes
		Vector3 m_pos;
        Vector3 m_front;
        Vector3 m_up;
        Vector3 m_right;
        Vector3 m_worldUp;

		// Eular Angles
        float m_yaw;
        float m_pitch;
		// Camera options
        float m_speed;
        float m_msensitivity;
        float m_zoom;

		void updateCam();
	};
}