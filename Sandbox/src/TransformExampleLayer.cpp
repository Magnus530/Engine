#include "TransformExampleLayer.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/AssetLoaders/ObjLoader.h"



TransformExampleLayer::TransformExampleLayer()
	: Layer("TransformTesting"), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
{
	m_Shader = m_ShaderLibrary.Load("assets/shaders/Flat.glsl");

	m_Scene = std::make_shared<Engine::Scene>();
	m_Entity = Engine::EntityInitializer::GetInstance().EntityInit("Cube", m_CubeVA, m_Scene);
	m_Entity.AddComponent<Engine::RendererComponent>(glm::vec4(0.5f, 0.2f, 0.6f, 1.f));

	auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
	Engine::TransformSystem::SetWorldPosition(transform, glm::vec3(-2, -1, 0));

	//FMOD initialize
	m_Audio = std::make_shared<Engine::AudioEngine>();
	m_Audio->init();
}

void TransformExampleLayer::OnUpdate(Engine::Timestep ts)
{
	// Update 
	m_PCameraController.OnUpdate(ts);
	// Begin Scene
	Engine::Renderer::BeginScene(m_PCameraController.GetCamera());

	// Set background color 
	Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Engine::RenderCommand::Clear();

	glm::mat4 projectionmatrix = m_PCameraController.GetCamera().GetProjectionMatrix();
	glm::mat4 viewmatrix = m_PCameraController.GetCamera().GetViewMatrix();

	// Changing color of the red channel - is multiplied in m_Shader's vertex shader 
	static float sin{};
	sin += ts;
	float tmp = sinf(sin);
	float fleeting = (sinf(sin) + 1) / 2;

	// ---- TRANSFORMATION TESTING ---------------------
	auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
		// Position
	Engine::TransformSystem::SetWorldPosition(transform, m_Position);

		// Rotation
	if (bAddWorldRotation) {
		Engine::TransformSystem::AddWorldRotation(transform, ts * m_RotationStrength, glm::vec3(0, 1.f, 0));
	}
	if (bAddLocalRotation) {
		Engine::TransformSystem::AddLocalRotation(transform, ts * m_RotationStrength, glm::vec3(1.f, 0, 0));
	}
	
	// Updating TransformComponent
	Engine::TransformSystem::UpdateMatrix(transform);

	// Draw Call for m_Obj
	auto shader = std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader);
	shader->UploadUniformFloat3("u_Color", m_ObjColor);
	shader->UploadUniformInt("u_ShowCustomColor", bShowCustomColor);

	Engine::Renderer::Submit(m_Shader, m_CubeVA, transform.m_Transform);

	// End Scene
	Engine::Renderer::EndScene();
}

void TransformExampleLayer::OnImGuiRender()
{
	// Transformation Demo
	ImGui::Begin("TransformTesting");
	if (ImGui::Button("Reset Transformations")) {
		m_Position *= 0.f;
	}
	ImGui::Separator();

	ImGui::Text("Position");
	ImGui::PushItemWidth(75.f);
	const char* format = "%.1f";
	//ImGui::Checkbox("Set World Position", &bSetWorldPosition);
	ImGui::SliderFloat("X", &m_Position.x, -10.f, 10.f, format);
	ImGui::SameLine();
	ImGui::SliderFloat("Y", &m_Position.y, -10.f, 10.f, format);
	ImGui::SameLine();
	ImGui::SliderFloat("Z", &m_Position.z, -10.f, 10.f, format);

	ImGui::Separator();

	ImGui::PushItemWidth(100.f);
	//ImGui::Checkbox("Add World Position", &bAddWorldPosition);
	//ImGui::Checkbox("Add Local Position", &bAddLocalPosition);
	//ImGui::SliderFloat("Position Strength", &m_PositionStrength, -1.f, 1.f);

	ImGui::Text("Rotation");
	ImGui::Checkbox("Add World Rotation", &bAddWorldRotation);
	ImGui::Checkbox("Add Local Rotation", &bAddLocalRotation);
	ImGui::SliderFloat("Rotation Strength", &m_RotationStrength, -1.f, 1.f);
	ImGui::Separator();

	ImGui::Text("Scale");
	ImGui::Checkbox("Add Scale", &bAddScale);
	ImGui::SliderFloat("Scale Strength", &m_ScaleStrength, -1.f, 1.f);
	//ImGui::Checkbox("Set Scale", &bSetScale);
	//ImGui::SliderFloat("Set Scale", &m_SetScale, 0.f, 3.f);
	ImGui::Separator();

	ImGui::Separator();
	ImGui::PushItemWidth(200.f);
	ImGui::ColorEdit3("Object color", glm::value_ptr(m_ObjColor));
	ImGui::Checkbox("Set custom color", &bShowCustomColor);
	ImGui::Separator();

	ImGui::End();
}
