#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
	class Camera
	{
	public:
		Camera() {}
		virtual ~Camera() {}

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3 position) { m_Position = position; RecalculateViewMatrix(); }

		glm::vec3 GetRotation() const { return m_Rotation; }
		void SetRotation(glm::vec3 rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetProjectionViewMatrix() const { return m_ProjectionViewMatrix; }

		glm::vec3& direction() { return m_Dir; }
		glm::vec3 right() { return glm::normalize(glm::cross(m_Dir, { 0,1,0 })); }
		glm::vec3 up() { return glm::normalize(glm::cross(right(), m_Dir)); }

		glm::vec3 m_TranslationDir = { 0.0f, 0.0f, 0.0f };

	protected:
		void RecalculateViewMatrix() 
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position);
				glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1, 0, 0)) *
				glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0, 1, 0)) *
				glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));

				m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Dir, { 0,1,0 });
			//m_ViewMatrix = glm::inverse(transform);
			m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
		}

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionViewMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };

		glm::vec3 m_Dir = { 0.0f, 0.0f, -1.0f };
	};
}