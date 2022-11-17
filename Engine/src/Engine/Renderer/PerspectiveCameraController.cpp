#include "epch.h"
#include "PerspectiveCameraController.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"

namespace Engine
{
	PerspectiveCameraController::PerspectiveCameraController(float fov, float aspectRatio, float nearPlane, float farPlane)
		: m_Camera(fov, aspectRatio, nearPlane, farPlane)
	{}

	void PerspectiveCameraController::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(E_KEY_A))
		{
			m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(E_KEY_D))
		{
			m_CameraPosition.x += m_CameraTranslationSpeed * ts;
		}

		if (Input::IsKeyPressed(E_KEY_Q))
		{
			m_CameraPosition.y += m_CameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(E_KEY_E))
		{
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
		}

		if (Input::IsKeyPressed(E_KEY_W))
		{
			m_CameraPosition.z -= m_CameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(E_KEY_S))
		{
			m_CameraPosition.z += m_CameraTranslationSpeed * ts;
		}

		if (Input::IsKeyPressed(E_KEY_R))
		{
			m_CameraRotation.x += m_CameraRotationSpeed * ts;
		}
		else if (Input::IsKeyPressed(E_KEY_T))
		{
			m_CameraRotation.x -= m_CameraRotationSpeed * ts;
		}

		if (Input::IsKeyPressed(E_KEY_F))
		{
			m_CameraRotation.y += m_CameraRotationSpeed * ts;
		}
		else if (Input::IsKeyPressed(E_KEY_G))
		{
			m_CameraRotation.y -= m_CameraRotationSpeed * ts;
		}

		if (Input::IsKeyPressed(E_KEY_V))
		{
			m_CameraRotation.y += m_CameraRotationSpeed * ts;
		}
		else if (Input::IsKeyPressed(E_KEY_B))
		{
			m_CameraRotation.y -= m_CameraRotationSpeed * ts;
		}


		m_Camera.SetRotation(m_CameraRotation);

		m_Camera.SetPosition(m_CameraPosition);
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