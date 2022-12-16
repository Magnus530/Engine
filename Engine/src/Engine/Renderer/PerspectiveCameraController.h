#pragma once

#include "Engine/Renderer/PerspectiveCamera.h"
#include "Engine/Core/Timestep.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"

namespace Engine
{
	class PerspectiveCameraController
	{
	public:
		PerspectiveCameraController(float fov, float aspectRatio, float nearPlane, float farPlane);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		PerspectiveCamera& GetCamera() { return *m_Camera; }
		const PerspectiveCamera& GetCamera() const { return *m_Camera; }

		glm::vec3 GetPos() { return m_CameraPosition; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

	private:
		std::shared_ptr<PerspectiveCamera> m_Camera;

		bool m_Rotation;
		glm::vec2 m_RenderwindowSize = { 1280, 720 };
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_CameraRotation = { 0.0f, 0.0f, 0.0f };
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 100.0f;

		glm::vec3 m_TranslationDir = { 0.0f, 0.0f, 0.0f };
	};
}