#include "epch.h"
#include "PerspectiveCameraController.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseButtonCodes.h"
#include "Platform/Windows/WindowsInput.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Engine
{
	PerspectiveCameraController::PerspectiveCameraController(float fov, float aspectRatio, float nearPlane, float farPlane)
		: m_Camera(std::make_shared<PerspectiveCamera>(fov, aspectRatio, nearPlane, farPlane))
	{}

	void PerspectiveCameraController::OnUpdate(Timestep ts)
	{
		glm::vec3 TranslationDir{ 0,0,0 };
		if (Input::IsKeyPressed(E_KEY_A))
		{
			m_TranslationDir -= m_Camera->right();
		}
		else if (Input::IsKeyPressed(E_KEY_D))
		{
			m_TranslationDir += m_Camera->right();
		}

		if (Input::IsKeyPressed(E_KEY_Q))
		{
			m_TranslationDir -= m_Camera->up();
		}
		else if (Input::IsKeyPressed(E_KEY_E))
		{
			m_TranslationDir += m_Camera->up();
		}

		if (Input::IsKeyPressed(E_KEY_W))
		{
			m_TranslationDir += m_Camera->direction();
		}
		else if (Input::IsKeyPressed(E_KEY_S))
		{
			m_TranslationDir -= m_Camera->direction();
		}
		//E_CORE_INFO("Camera dir: {0}, {1}, {2}", m_Camera.direction().x, m_Camera.direction().y, m_Camera.direction().z);

		static bool bFirstClick = true;
		static glm::vec2 center{ m_RenderwindowSize.x / 2.f, m_RenderwindowSize.y / 2.f };
		glm::vec2 mousePos;
		glm::vec2 delta;
		const float cameraSensitivity = 10.f;
		if (Engine::Input::IsMouseButtonPressed(E_MOUSE_BUTTON_2))
		{
			//center = renderwindowCenter;
			if (!bFirstClick)
			{
				auto [x, y] = Engine::Input::GetMousePosition();
				mousePos.x = x;
				mousePos.y = y;
				delta = mousePos - center;
				glm::vec3 newDir = glm::rotate(m_Camera->direction(),
					glm::radians(-delta.y * cameraSensitivity * static_cast<float>(ts)), m_Camera->right());
				if (!(glm::angle(newDir, { 0,1,0 }) <= glm::radians(5.f) || glm::angle(newDir, { 0,-1,0 }) <= glm::radians(5.f)))
				{
					m_Camera->direction() = glm::normalize(newDir);
				}

				m_Camera->direction() = glm::normalize(glm::rotate(m_Camera->direction(), glm::radians(-delta.x * cameraSensitivity * static_cast<float>(ts)), { 0, 1, 0 }));
				if (glm::length(m_TranslationDir) > 0.01f)
				{
					TranslationDir = glm::normalize(TranslationDir);
					m_Camera->m_TranslationDir = TranslationDir;
				}
			}
			bFirstClick = false;
			Input::SetMousePos( center.x, center.y );
		}
		else
		{
			bFirstClick = true;
		}

		if (glm::length(m_TranslationDir) > 0)
		{
			m_CameraTranslationSpeed = std::clamp(m_CameraTranslationSpeed, 0.2f, FLT_MAX);
			m_CameraPosition += m_TranslationDir * static_cast<float>(ts) * m_CameraTranslationSpeed;
			m_TranslationDir = glm::vec3{ 0.f };
		}

		m_Camera->SetRotation(m_CameraRotation);
		m_Camera->SetPosition(m_CameraPosition);
	}

	void PerspectiveCameraController::OnEvent(Event& e)
	{
	}

	bool PerspectiveCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		return false;
	}

	bool PerspectiveCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		return false;
	}
}