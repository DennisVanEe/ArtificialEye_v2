#pragma once

#include <glad/glad.h>
#include <vector>

#include "../Types.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace ee
{
	enum class CameraDir
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

    // All floats because it is mainly used for OpenGL rendering
	class Camera
	{
	public:
		Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch);

        glm::mat4 viewMatrix() const;

		void processKBInput(CameraDir direction, float deltaTime);

		void processMInput(float xOffset, float yOffset, GLboolean constrainPitch = true);
		void processSInput(float scrOffset);

        glm::vec3 getPosition() const;

	private:

		// Camera Attributes
        glm::vec3 m_pos;
        glm::vec3 m_front;
        glm::vec3 m_up;
        glm::vec3 m_right;
        glm::vec3 m_worldUp;

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