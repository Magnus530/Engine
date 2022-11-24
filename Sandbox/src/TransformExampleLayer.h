#pragma once
#include <Engine.h>

class TransformExampleLayer : public Engine::Layer
{
public:
	TransformExampleLayer();
	//~TransformExampleLayer(){}

	//void OnAttach() override;
	//void OnDetach() override;
	void OnUpdate(Engine::Timestep ts) override;
	void OnImGuiRender() override;
	//void OnEvent(Engine::Event& event) override{}

private:
	Engine::PerspectiveCameraController m_PCameraController;
	Engine::ShaderLibrary m_ShaderLibrary;
	std::shared_ptr<Engine::Shader> m_Shader;

	std::shared_ptr<Engine::VisualObject> m_Obj;

private: // Transform testing
	//bool bSetWorldPosition{};
	glm::vec3 m_Position{};
	bool bAddWorldPosition{};
	bool bAddLocalPosition{};
	float m_PositionStrength{ 0.f };

	bool bAddWorldRotation{};
	bool bAddLocalRotation{};
	float m_RotationStrength{ 0.f };

	bool bAddScale{};
	float m_ScaleStrength{ 0.f };
	bool bSetScale{};
	float m_SetScale{ 1.f };

	glm::vec3 m_ObjColor;
	bool bShowCustomColor{};
};