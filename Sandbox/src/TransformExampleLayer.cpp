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
	m_Shader = m_ShaderLibrary.Load("assets/shaders/Flat.glsl");

	/* Loading obj */
	std::vector<Engine::Vertex> vertices;
	std::vector<uint32_t> indices;
	Engine::ObjLoader::ReadFile("Monkey", vertices, indices);
	m_Scene = std::make_shared<Engine::Scene>();
	m_Entity = Engine::EntityInitializer::GetInstance().EntityInit("Cube", m_CubeVA, m_Scene);
	m_Entity.AddComponent<Engine::RendererComponent>(glm::vec4(0.5f, 0.2f, 0.6f, 1.f));

	auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
	//Engine::TransformSystem::SetWorldPosition(transform, glm::vec3(-3, -4, 0));
	//Engine::TransformSystem::SetWorldPosition(transform, glm::vec3(-3, -4, 0));
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
	// Changing color of the red channel - is multiplied in m_Shader's vertex shader 
	static float sin{};
	sin += ts;
	float tmp = sinf(sin);
	float fleeting = (sinf(sin) + 1) / 2;

	// ---- TRANSFORMATION TESTING ---------------------
	auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
	//Engine::TransformSystem::SetWorldPosition(transform, m_Position);

		// Rotation
	if (bAddWorldRotation) {
		Engine::TransformSystem::AddWorldRotation(transform, ts * m_RotationStrength, glm::vec3(0, 1.f, 0));
	}
	if (bAddLocalRotation) {
		Engine::TransformSystem::AddLocalRotation(transform, ts * m_RotationStrength, glm::vec3(1.f, 0, 0));
	}
	
	// Updating TransformComponent

	// Draw Call for m_Obj
	auto shader = std::dynamic_pointer_cast<Engine::OpenGLShader>(m_Shader);
	shader->UploadUniformFloat3("u_Color", m_ObjColor);
	shader->UploadUniformInt("u_ShowNormals", bShowNormals);

	Engine::Renderer::Submit(Engine::ShaderType::Flat, m_Shader, m_CubeVA, m_Entity);
	//Engine::Renderer::Submit(m_Shader, m_CubeVA, transform.m_Transform);

	// End Scene
	Engine::Renderer::EndScene();
}

void TransformExampleLayer::OnImGuiRender()
{
	// Transformation Demo
	ImGui::Begin("TransformTesting");
	if (ImGui::Button("Reset Transformations")) {
		//m_Obj->Reset();
		m_Position *= 0.f;
	}
	ImGui::Separator();

	auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();

	ImGui::PushID(0);
	ImGui::Text("Position");
	ImGui::PushItemWidth(75.f);
	const char* format = "%.1f";

	if (ImGui::SliderFloat("X", &m_Position.x, -10.f, 10.f, format))
		Engine::TransformSystem::SetWorldPosition(transform, m_Position);
	ImGui::SameLine();
	if (ImGui::SliderFloat("Y", &m_Position.y, -10.f, 10.f, format))
		Engine::TransformSystem::SetWorldPosition(transform, m_Position);
	ImGui::SameLine();
	if (ImGui::SliderFloat("Z", &m_Position.z, -10.f, 10.f, format))
		Engine::TransformSystem::SetWorldPosition(transform, m_Position);
	ImGui::PopID();
	ImGui::Separator();

	ImGui::PushItemWidth(100.f);

	ImGui::Text("Rotation");
	ImGui::Checkbox("Add World Rotation", &bAddWorldRotation);
	ImGui::Checkbox("Add Local Rotation", &bAddLocalRotation);
	ImGui::SliderFloat("Rotation Strength", &m_RotationStrength, -1.f, 1.f);
	ImGui::Separator();


	ImGui::PushID(1);
	if (ImGui::SliderFloat("Scale", &m_SetScale, 0.f, 5.f))
	{
		Engine::TransformSystem::SetScale(m_Entity.GetComponent<Engine::TransformComponent>(), m_SetScale);
	}
	ImGui::Text("Scale");
	static glm::vec3 Scale(1.f);
	if (ImGui::SliderFloat("X", &Scale.x, 0, 10.f, format))
		Engine::TransformSystem::SetScale(m_Entity.GetComponent<Engine::TransformComponent>(), Scale);
	ImGui::SameLine();
	if (ImGui::SliderFloat("Y", &Scale.y, 0, 10.f, format))
		Engine::TransformSystem::SetScale(m_Entity.GetComponent<Engine::TransformComponent>(), Scale);
	ImGui::SameLine();
	if (ImGui::SliderFloat("Z", &Scale.z, 0, 10.f, format))
		Engine::TransformSystem::SetScale(m_Entity.GetComponent<Engine::TransformComponent>(), Scale);
	ImGui::PopID();
	ImGui::Separator();

	ImGui::Separator();
	ImGui::PushItemWidth(200.f);
	ImGui::ColorEdit3("Object color", glm::value_ptr(m_Entity.GetComponent<Engine::RendererComponent>().m_Color));
	ImGui::Checkbox("Show Normals", &m_Entity.GetComponent<Engine::RendererComponent>().m_bCustomColor);
	ImGui::Separator();

	ImGui::End();
}
