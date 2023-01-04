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

	
	ImGui::ShowDemoWindow();
}
