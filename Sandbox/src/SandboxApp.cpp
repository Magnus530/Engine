/* The game engine series by The Cherno on Youtube, was a helpful source when creating this engine.
https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=1 */

#include <Engine.h>
#include "Engine/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Renderer/VertexArray.h"
#include "Engine/AssetInit/ObjLoader.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Engine/Scene/EntityInitializer.h"
#include <unordered_map>

// Layers
#include "PathfindingLayer.h"
#include "TransformExampleLayer.h"

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_OCameraController(1280.0f / 720.0f, true), m_PCameraController(50.0f, 1280.0f / 720.0f, 0.01f, 1000.0f)
	{
		Engine::Renderer::RenderInit();
		m_ActiveScene = std::make_shared<Engine::Scene>();

		m_Texture = Engine::Renderer::CreateTexture("Chess", "assets/textures/checkerboard.png", m_ActiveScene);
		m_WolfLogoTexture = Engine::Renderer::CreateTexture("Wolf", "assets/textures/wolf.png", m_ActiveScene);
		m_WhiteTexture = Engine::Renderer::CreateTexture("White", "assets/textures/white.png", m_ActiveScene);

		//Create entities here

		m_LightEntity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Flat, "Sphere", m_SphereVA, m_ActiveScene, glm::vec3{ 1.0f, 1.0f, 0.0f });
		m_LightEntity.AddComponent<Engine::LightComponent>(float{ 0.2f }, float { 2.f });
		Engine::TransformSystem::SetWorldPosition(m_LightEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 0.0f, 3.0f, 1.0f });
		Engine::TransformSystem::SetScale(m_LightEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 0.5f, 0.5f, 0.5f });

		m_CubeEntity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Texture, "Cube", m_CubeVA, m_ActiveScene, glm::vec3{ 1.0f, 1.0f, 1.0f }, m_WolfLogoTexture);

		m_PlaneEntity = Engine::EntityInitializer::GetInstance().EntityInit(Engine::ShaderType::Phong, "Plane", m_PlaneVA, m_ActiveScene, glm::vec3{ 0.0f, 0.0f, 1.0f });
		Engine::TransformSystem::SetScale(m_PlaneEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 3.0f, 3.0f, 3.0f });

		m_Audio = std::make_shared<Engine::AudioEngine>();
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		m_PCameraController.OnUpdate(ts);
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();

		Engine::Renderer::BeginScene(m_PCameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		m_Audio->update(ts);


		for (auto& it = m_ActiveScene->m_EntityMap.begin(); it != m_ActiveScene->m_EntityMap.end(); it++)
		{
			Engine::Renderer::Submit(*(it)->second);

			if ((it)->second->HasComponent<Engine::PhongMaterialComponent>())
			{
				Engine::LightSystem::UpdateLight((it)->second->GetComponent<Engine::PhongMaterialComponent>(), m_LightEntity, m_PCameraController);
			}
		}

		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");

		int i = 0;
		std::vector<std::string> m_TextureNames;
		std::vector<const char*> m_TexNames;

		for (auto& it = m_ActiveScene->m_Textures.begin(); it != m_ActiveScene->m_Textures.end(); it++)
		{
			m_TextureNames.push_back((it)->first);
		}

		for (std::string const& str : m_TextureNames)
		{
			m_TexNames.push_back(str.data());
		}

		static int currentTexture = 0;
		static int currentPhongTexture = 0;
		const char* comboTexPreviewValue = m_TexNames[currentTexture];
		const char* comboPhongPreviewValue = m_TexNames[currentPhongTexture];

		for (auto& it = m_ActiveScene->m_EntityMap.begin(); it != m_ActiveScene->m_EntityMap.end(); it++)
		{
			ImGui::Separator();
			auto& tag = (it)->second->GetComponent<Engine::TagComponent>().Tag;
			ImGui::Text("%s", tag.c_str());

			switch ((it)->second->GetComponent<Engine::MaterialComponent>().m_ShaderType)
			{
				case Engine::ShaderType::Flat:
				{
					ImGui::PushID(i);
					glm::vec4& objColor = (it)->second->GetComponent<Engine::FlatMaterialComponent>().m_Color;
					ImGui::ColorEdit4("Color", glm::value_ptr(objColor));
					ImGui::Separator();
					ImGui::Checkbox("Show Custom Color", &(it)->second->GetComponent<Engine::RendererComponent>().m_bCustomColor);
					ImGui::Separator();
					ImGui::PopID();
					i++;
					break;
				}
				case Engine::ShaderType::Texture:
				{
					ImGui::PushID(i);
					ImGui::PushItemWidth(200.f);
					if (ImGui::BeginCombo("Texture Selection", comboTexPreviewValue))
					{
						int j = 0;
						for (auto& jt = m_ActiveScene->m_Textures.begin(); jt != m_ActiveScene->m_Textures.end(); jt++)
						{
							const bool is_Selected = (currentTexture == j);
							if (ImGui::Selectable(m_TextureNames[j].c_str(), is_Selected))
							{
								it->second->GetComponent<Engine::TextureMaterialComponent>().m_Tex = jt->second;
								currentTexture = j;
							}

							if (is_Selected)
							{
								ImGui::SetItemDefaultFocus();
							}
							j++;
						}
						ImGui::EndCombo();
					}
					ImGui::Separator();
					ImGui::PopID();
					i++;
					break;
				}
				case Engine::ShaderType::Phong:
				{
					ImGui::PushID(i);
					ImGui::PushItemWidth(200.f);
					if (ImGui::BeginCombo("Phong Texture Selection", comboPhongPreviewValue))
					{
						int j = 0;
						for (auto& jt = m_ActiveScene->m_Textures.begin(); jt != m_ActiveScene->m_Textures.end(); jt++)
						{
							const bool is_Selected = (currentTexture == j);
							if (ImGui::Selectable(m_TextureNames[j].c_str(), is_Selected))
							{
								it->second->GetComponent<Engine::PhongMaterialComponent>().m_Tex = jt->second;
								currentTexture = j;
							}

							if (is_Selected)
							{
								ImGui::SetItemDefaultFocus();
							}
							j++;
						}
						ImGui::EndCombo();
					}
					ImGui::Separator();
					glm::vec4& objColor = (it)->second->GetComponent<Engine::PhongMaterialComponent>().m_Color;
					ImGui::ColorEdit4("Color", glm::value_ptr(objColor));
					ImGui::Separator();
					ImGui::Checkbox("Show Custom Color", &(it)->second->GetComponent<Engine::RendererComponent>().m_bCustomColor);
					ImGui::Separator();
					ImGui::PopID();
					i++;
					break;
				}
			}
		}

		ImGui::End();
	}

	void OnEvent(Engine::Event& e) override
	{
		m_PCameraController.OnEvent(e);
	}

private:
	std::shared_ptr<Engine::Shader> m_Shader;

	std::shared_ptr<Engine::AudioEngine> m_Audio;
	std::shared_ptr<Engine::Shader> m_FlatColorShader;
	std::shared_ptr<Engine::VertexArray> m_PlaneVA, m_CubeVA, m_SphereVA;

	std::shared_ptr<Engine::Texture2D> m_Texture, m_WolfLogoTexture, m_WhiteTexture;

	Engine::PerspectiveCameraController m_PCameraController;
	Engine::OrthographicCameraController m_OCameraController;

	std::shared_ptr<Engine::Scene> m_ActiveScene;

	// Entities
	Engine::Entity m_CubeEntity;
	Engine::Entity m_PlaneEntity;
	Engine::Entity m_LightEntity;
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		//PushLayer(new PathfindingLayer());
		//PushLayer(new TransformExampleLayer());
		
		SetCurrentLayer(0);
		SetGuiLayerNames();
	}

	~Sandbox()
	{}

private:
	std::shared_ptr<Engine::Layer> m_CurrentLayer;
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}
