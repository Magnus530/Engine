#include "TransformExampleLayer.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/AssetInit/ObjLoader.h"
#include "Engine/Renderer/VertexArray.h"


TransformExampleLayer::TransformExampleLayer()
	: Layer("TransformTesting"), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
{
	m_Scene = std::make_shared<Engine::Scene>();
	
	m_Entity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Flat, "Monkey", m_MonkeyVA, m_Scene);
	m_Entity.GetComponent<Engine::RendererComponent>().m_bCustomColor = true;
	
	m_Audio = std::make_shared<Engine::AudioEngine>();
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

	m_Audio->update(ts);

	Engine::Renderer::Submit(m_Entity);

	// End Scene
	Engine::Renderer::EndScene();
}

void TransformExampleLayer::OnImGuiRender()
{

	// Transformation Demo
	std::shared_ptr<Engine::ImGuiSystem> imGuiPtr = std::make_shared<Engine::ImGuiSystem>();
	imGuiPtr->GuiEntitySettings(m_Scene);

	//auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
	//ImGui::PushItemWidth(100.f);
	//ImGui::Separator();

	//// POSITION
	//ImGui::PushID(1);
	//ImGui::Text("Position");
	//if (ImGui::DragFloat("X", &m_Position.x, 0.01f))
	//	Engine::TransformSystem::SetWorldPosition(transform, m_Position);
	//ImGui::SameLine();
	//if (ImGui::DragFloat("Y", &m_Position.y, 0.01f))
	//	Engine::TransformSystem::SetWorldPosition(transform, m_Position);
	//ImGui::SameLine();
	//if (ImGui::DragFloat("Z", &m_Position.z, 0.01f))
	//	Engine::TransformSystem::SetWorldPosition(transform, m_Position);
	//ImGui::PopID();

	//static glm::vec3 Rotator = transform.GetRotator();

	//// ROTATION
	//ImGui::PushID(2);
	//ImGui::Text("Set Entity Rotation");
	//if (ImGui::DragFloat("Pitch", &Rotator.x, 0.01f))
	//	Engine::TransformSystem::SetRotation(transform, Rotator);
	//ImGui::SameLine();
	//if (ImGui::DragFloat("Yaw", &Rotator.y, 0.01f))
	//	Engine::TransformSystem::SetRotation(transform, Rotator);
	//ImGui::SameLine();
	//if (ImGui::DragFloat("Roll", &Rotator.z, 0.01f))
	//	Engine::TransformSystem::SetRotation(transform, Rotator);
	//ImGui::PopID();


	//// SCALE
	//ImGui::Text("Scale");
	//ImGui::PushID(3);
	//if (ImGui::DragFloat("X", &m_Scale.x, 0.01f))
	//	Engine::TransformSystem::SetScale(transform, m_Scale);
	//ImGui::SameLine();
	//if (ImGui::DragFloat("Y", &m_Scale.y, 0.01f))
	//	Engine::TransformSystem::SetScale(transform, m_Scale);
	//ImGui::SameLine();
	//if (ImGui::DragFloat("Z", &m_Scale.z, 0.01f))
	//	Engine::TransformSystem::SetScale(transform, m_Scale);
	//ImGui::PopID();


	//// RESET TRANSFORMS
	//if (ImGui::Button("Reset Transformations")) 
	//{
	//	m_Position *= 0.f;
	//	m_Rotator *= 0.f;
	//	m_Scale = { 1,1,1 };
	//	m_SetScale = 1.f;
	//	Engine::TransformSystem::ResetTransforms(transform);
	//}
	//ImGui::Separator();
}
