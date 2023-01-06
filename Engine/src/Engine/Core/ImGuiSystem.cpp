#include "epch.h"
#include "ImGuiSystem.h"
#include "Engine.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Scene/Systems/PathfindingSystem.h"

namespace Engine
{
	void ImGuiSystem::LayerSelection(Application* app, std::vector<std::string>& layernames)
	{
		ImGui::Begin("Layer Selection");

		static const char* currentLayer{ layernames[0].c_str() };

		ImGui::PushItemWidth(200.f);
		if (ImGui::BeginCombo("Layer Selection", currentLayer))
		{
			for (uint32_t i{}; i < app->GetLayerCount(); i++)
			{
				bool b{};
				if (ImGui::Selectable(layernames[i].c_str(), &b)) {
					currentLayer = layernames[i].c_str();
					app->SetCurrentLayer(i);
				}
			}
			ImGui::EndCombo();
		}

		ImGui::End();
	}

	void ImGuiSystem::GuiEntitySettings(std::shared_ptr<Engine::Scene>& scene)
	{
		ImGui::Begin("Entity Settings");

		int i = 0;
		int id = 0;
		std::vector<std::string> textureNames;
		std::vector<std::string> skyboxNames;
		std::vector<std::string> shaderNames = { "Flat", "Texture", "Phong" };

		for (auto& it = scene->m_Textures.begin(); it != scene->m_Textures.end(); it++)
		{
			textureNames.push_back((it)->first);
		}

		for (auto& it = scene->m_Skyboxes.begin(); it != scene->m_Skyboxes.end(); it++)
		{
			skyboxNames.push_back((it)->first);
		}

		for (auto& it = scene->m_Entities.begin(); it != scene->m_Entities.end(); it++)
		{
			ImGui::Separator();
			auto& tag = (it)->second->GetComponent<Engine::TagComponent>().Tag;

			ImFont* font = new ImFont(*ImGui::GetDefaultFont());
			font->FontSize = 20.f;
			ImGui::PushFont(font);
			ImGui::SetWindowFontScale(1.5f);
			ImGui::Text("%s", tag.c_str());
			ImGui::SetWindowFontScale(1.f);
			ImGui::PopFont();

			switch ((it)->second->GetComponent<Engine::MaterialComponent>().m_ShaderType)
			{
			case Engine::ShaderType::Flat:
			{
				static int currentShader = 0;
				const char* comboShaderPreviewValue = shaderNames[0].c_str();

				ImGui::PushID(i);
				glm::vec4& objColor = (it)->second->GetComponent<Engine::FlatMaterialComponent>().m_Color;
				ImGui::ColorEdit4("Color", glm::value_ptr(objColor));
				ShaderSelect(it->second, currentShader, comboShaderPreviewValue, shaderNames);
				ImGui::Checkbox("Show Custom Color", &(it)->second->GetComponent<Engine::RendererComponent>().m_bCustomColor);
				ImGui::PopID();
				i++;
				break;
			}
			case Engine::ShaderType::Texture:
			{
				static int currentTexture = 0;
				static int currentShader = 0;
				const char* comboTexPreviewValue = EntityNameCompare((it)->second->GetComponent<Engine::TextureMaterialComponent>().m_Tex.first, textureNames);
				const char* comboShaderPreviewValue = shaderNames[1].c_str();

				ImGui::PushID(i);
				ImGui::PushItemWidth(200.f);
				if (ImGui::BeginCombo("Texture Selection", comboTexPreviewValue))
				{
					int j = 0;
					for (auto& jt = scene->m_Textures.begin(); jt != scene->m_Textures.end(); jt++)
					{
						const bool is_Selected = (currentTexture == j);
						if (ImGui::Selectable(textureNames[j].c_str(), is_Selected))
						{
							it->second->GetComponent<Engine::TextureMaterialComponent>().m_Tex.first = jt->first;
							it->second->GetComponent<Engine::TextureMaterialComponent>().m_Tex.second = jt->second;
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
				ShaderSelect(it->second, currentShader, comboShaderPreviewValue, shaderNames);
				ImGui::PopID();
				i++;
				break;
			}
			case Engine::ShaderType::Phong:
			{		
				static int currentPhongTexture = 0;
				static int currentShader = 0;
				const char* comboPhongPreviewValue = EntityNameCompare((it)->second->GetComponent<Engine::PhongMaterialComponent>().m_Tex.first, textureNames);
				const char* comboShaderPreviewValue = shaderNames[2].c_str();

				ImGui::PushID(i);
				ImGui::PushItemWidth(200.f);
				if (ImGui::BeginCombo("Phong Texture Selection", comboPhongPreviewValue))
				{
					int j = 0;
					for (auto& jt = scene->m_Textures.begin(); jt != scene->m_Textures.end(); jt++)
					{
						const bool is_Selected = (currentPhongTexture == j);
						if (ImGui::Selectable(textureNames[j].c_str(), is_Selected))
						{
							it->second->GetComponent<Engine::PhongMaterialComponent>().m_Tex.first = jt->first;
							it->second->GetComponent<Engine::PhongMaterialComponent>().m_Tex.second = jt->second;
							currentPhongTexture = j;
						}

						if (is_Selected)
						{
							ImGui::SetItemDefaultFocus();
						}
						j++;
					}
					ImGui::EndCombo();
				}
				glm::vec4& objColor = (it)->second->GetComponent<Engine::PhongMaterialComponent>().m_Color;
				glm::vec3& objAColor = (it)->second->GetComponent<Engine::PhongMaterialComponent>().m_AmbientColor;
				ImGui::ColorEdit4("Color", glm::value_ptr(objColor));
				ImGui::ColorEdit3("Ambient Color", glm::value_ptr(objAColor));
				ShaderSelect(it->second, currentShader, comboShaderPreviewValue, shaderNames);
				ImGui::Checkbox("Show Custom Color", &(it)->second->GetComponent<Engine::RendererComponent>().m_bCustomColor);
				ImGui::PopID();
				i++;
				break;
			}
			case ShaderType::Skybox:
			{
				static int currentSkyboxTexture = 0;
				const char* comboSkyPreviewValue = EntityNameCompare((it)->second->GetComponent<Engine::SkyboxMaterialComponent>().m_CubeTex.first, skyboxNames);

				ImGui::PushID(i);
				ImGui::PushItemWidth(200.f);
				if (ImGui::BeginCombo("Skybox Selection", comboSkyPreviewValue))
				{
					int j = 0;
					for (auto& jt = scene->m_Skyboxes.begin(); jt != scene->m_Skyboxes.end(); jt++)
					{
						const bool is_Selected = (currentSkyboxTexture == j);
						if (ImGui::Selectable(skyboxNames[j].c_str(), is_Selected))
						{
							it->second->GetComponent<Engine::SkyboxMaterialComponent>().m_CubeTex.first = jt->first;
							it->second->GetComponent<Engine::SkyboxMaterialComponent>().m_CubeTex.second = jt->second;
							currentSkyboxTexture = j;
						}

						if (is_Selected)
						{
							ImGui::SetItemDefaultFocus();
						}
						j++;
					}
					ImGui::EndCombo();
				}
				ImGui::PopID();
				i++;
				break;
			}
			}

			GuiEntitySettings_Transform(scene.get(), id, *it->second);
			if (it->second->HasComponent<PathfindingComponent>())
				GuiEntitySettings_Pathfinding(scene.get(), id, *it->second);
			if (it->second->HasComponent<ObstructionSphereComponent>())
				GuiEntitySettings_Obstruction(scene.get(), id, *it->second);

			id++;
			ImGui::Separator();
		}

		ImGui::End();
	}

	void ImGuiSystem::GuiPathfindingGridSettings(std::shared_ptr<Engine::Scene>& scene)
	{
		ImGui::Begin("Pathfinding Node Grid Creation");
		int id{std::numeric_limits<int>::max()};
		ImGui::PushItemWidth(75.f);
		id--;
		ImVec2 ButtonSize(130, 80);

		glm::vec3& Location = scene->m_PathfindingNodeGrid->m_Location;
		glm::ivec3& Extent = scene->m_PathfindingNodeGrid->m_Extent;
		int& Resolution = scene->m_PathfindingNodeGrid->Resolution;

		if (ImGui::Button("Update\nPathfindingGrid", ButtonSize))
		{
			glm::vec3 l = Location;
			glm::ivec3 e = Extent;
			int r = Resolution;
			bool b = scene->m_PathfindingNodeGrid->bRenderNodegrid;

			NodeGridSystem::CreateGridAtLocation(scene.get(), l, e, r, b);
			scene->UpdateObstructionsToNewGrid();
		}
		ImGui::PushID(id--);
		ImGui::Text("Extent");
		ImGui::DragInt("X", &Extent.x, 0.05f);
		ImGui::SameLine();
		ImGui::DragInt("Y", &Extent.y, 0.05f);
		ImGui::SameLine();
		ImGui::DragInt("Z", &Extent.z, 0.05f);
		ImGui::PopID();

		ImGui::PushID(id--);
		ImGui::DragInt("Grid Resolution", &Resolution, 0.5f, 1, 10);
		ImGui::PopID();

		ImGui::PushID(id--);
		ImGui::Checkbox("Render Nodegrid", &scene->m_PathfindingNodeGrid->bRenderNodegrid);
		ImGui::PopID();

		static int obstructionAmount{ 50 };
		if (ImGui::Button("Create\nObstructions", ButtonSize)) {
			scene->ClearObstructions();
			auto& player = scene->m_Entities["Monkey"]->GetComponent<TransformComponent>();
			scene->CreateSceneObstructions(obstructionAmount, player.GetLocation(), LoadObjectGetVertexArray("Rock"));
		}
		ImGui::SameLine();
		ImGui::DragInt("Amount of Obstructions", &obstructionAmount, 1, 0, 1000);
		if (ImGui::Button("Delete\nObstruction", ButtonSize - ImVec2(0, 30)))
			scene->ClearObstructions();

		ImGui::End();
	}

	const char* ImGuiSystem::EntityNameCompare(const std::string& name, std::vector<std::string>& names)
	{
		static int tempNameIndex = 0;
		const char* tempComboVal = 0;

		for (int i = 0; i < names.size(); i++)
		{
			if (name.c_str() == names[i])
			{
				tempNameIndex = i;
			}
		}

		return tempComboVal = names[tempNameIndex].c_str();
	}

	void ImGuiSystem::ShaderSelect(std::shared_ptr<Engine::Entity>& entity, int& currShader, const char* comboPreview, std::vector<std::string>& names)
	{
		ImGui::PushItemWidth(200.f);
		if (ImGui::BeginCombo("Shader Selection", comboPreview))
		{
			int k = 0;
			for (int j = 0; j < names.size(); j++)
			{
				const bool is_Selected = (currShader == k);
				if (ImGui::Selectable(names[k].c_str(), is_Selected))
				{
					switch (entity->GetComponent<MaterialComponent>().m_ShaderType)
					{
					case Engine::ShaderType::Flat:
					{
						if (names[k] == "Texture")
						{
							entity->GetComponent<Engine::MaterialComponent>().m_ShaderType = Engine::ShaderType::Texture;
							entity->RemoveComponent<Engine::FlatMaterialComponent>();
							entity->AddComponent<Engine::TextureMaterialComponent>("White", Engine::Texture2D::Create("assets/textures/white.png"));
							currShader = k;
							break;
						}
						else if (names[k] == "Phong")
						{
							entity->GetComponent<Engine::MaterialComponent>().m_ShaderType = Engine::ShaderType::Phong;
							entity->RemoveComponent<Engine::FlatMaterialComponent>();
							entity->AddComponent<Engine::PhongMaterialComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, "White", Engine::Texture2D::Create("assets/textures/white.png"));
currShader = k;
break;
						}
						break;
					}
					case Engine::ShaderType::Texture:
					{
						if (names[k] == "Flat")
						{
							entity->GetComponent<Engine::MaterialComponent>().m_ShaderType = Engine::ShaderType::Flat;
							entity->RemoveComponent<Engine::TextureMaterialComponent>();
							entity->AddComponent<Engine::FlatMaterialComponent>();
							currShader = k;
							break;
						}
						else if (names[k] == "Phong")
						{
							entity->GetComponent<Engine::MaterialComponent>().m_ShaderType = Engine::ShaderType::Phong;
							entity->RemoveComponent<Engine::TextureMaterialComponent>();
							entity->AddComponent<Engine::PhongMaterialComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, "White", Engine::Texture2D::Create("assets/textures/white.png"));
							currShader = k;
							break;
						}
						break;
					}
					case Engine::ShaderType::Phong:
					{
						if (names[k] == "Flat")
						{
							entity->GetComponent<Engine::MaterialComponent>().m_ShaderType = Engine::ShaderType::Flat;
							entity->RemoveComponent<Engine::PhongMaterialComponent>();
							entity->AddComponent<Engine::FlatMaterialComponent>();
							currShader = k;
							//Engine::FlatShaderState* flatStatePtr = new Engine::FlatShaderState;
							//m_RContextPtr->TransitionTo(flatStatePtr);
							break;
						}
						else if (names[k] == "Texture")
						{
							entity->GetComponent<Engine::MaterialComponent>().m_ShaderType = Engine::ShaderType::Texture;
							entity->RemoveComponent<Engine::PhongMaterialComponent>();
							entity->AddComponent<Engine::TextureMaterialComponent>("White", Engine::Texture2D::Create("assets/textures/white.png"));
							currShader = k;
							break;
						}
						break;
					}
					}
				}

				if (is_Selected)
				{
					ImGui::SetItemDefaultFocus();
				}
				k++;
			}
			ImGui::EndCombo();
		}
	}

	void ImGuiSystem::GuiAudioSettings(std::shared_ptr<Engine::AudioEngine>& audio)
	{
		ImGui::Begin("Audio Settings");
		ImVec2 ButtonSize(130, 80);
		//

		// Generate samples and plot them
		float samples[9001];
		for (int n = 0; n < 9001; n++)
			samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 5.f);
		ImGui::PlotLines("Samples that are over 9000", samples, 9001);

		std::string ambience = "assets/audio/Forest_amb.wav";
		std::string musicPath1 = "assets/audio/Music.wav";
		if (ImGui::Button("Reset to default", ButtonSize))
		{
			audio->stopAllChannels();
			audio->setEnvironmentReverb(FMOD_PRESET_OFF, glm::vec3(), 0.f, 0.1f);
		}
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("Reset to default causes to stop all sounds and reset the environment reverb that is put in the game.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		if (ImGui::Button("Play music"))
		{
			audio->playSound(musicPath1, glm::vec3(), -15.f, 1.f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Play Environment"))
		{
			audio->playSound(ambience, glm::vec3(), -10.f, 1.f);
		}
		if (ImGui::Button("Add Reverb"))
		{
			audio->setEnvironmentReverb(FMOD_PRESET_FOREST, glm::vec3(), 0.f, 1000.f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop reverb"))
		{
			audio->setEnvironmentReverb(FMOD_PRESET_OFF, glm::vec3(), 0.f, 0.1f);
		}

		ImGui::End();
	}
	
	void ImGuiSystem::GuiEntitySettings_Transform(Scene* scene, int& id, Entity& m_Entity)
	{
		auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
		ImGui::PushItemWidth(100.f);

		glm::vec3 Location	= transform.GetLocation();
		glm::vec3& Rotator	= transform.m_Rotator;
		glm::vec3 Scale		= transform.GetScale();

		// POSITION
		ImGui::PushID(id++);
		ImGui::Text("Position");
		bool b{};
		bool x, y, z;
		if (x = ImGui::DragFloat("X", &Location.x, 0.05f))
			Engine::TransformSystem::SetWorldPosition(transform, Location);
		ImGui::SameLine();
		if (y = ImGui::DragFloat("Y", &Location.y, 0.05f))
			Engine::TransformSystem::SetWorldPosition(transform, Location);
		ImGui::SameLine();
		if (z = ImGui::DragFloat("Z", &Location.z, 0.05f))
			Engine::TransformSystem::SetWorldPosition(transform, Location);
		ImGui::PopID();

		// Update Obstruction if entity contains Obstruction Component
		b = x || y || z;
		if (b && m_Entity.HasComponent<Engine::ObstructionSphereComponent>()) {
			auto& obs = m_Entity.GetComponent<Engine::ObstructionSphereComponent>();
			Engine::NodeGridSystem::UpdateObstructionSphere(scene, obs.m_ID, obs.m_radius, Location);
		}


		// ROTATION
		ImGui::PushID(id++);
		ImGui::Text("Set Entity Rotation");
		if (ImGui::DragFloat("Pitch", &Rotator.x, 0.1f))
			Engine::TransformSystem::SetRotation(transform, Rotator);
		ImGui::SameLine();
		if (ImGui::DragFloat("Yaw", &Rotator.y, 0.1f))
			Engine::TransformSystem::SetRotation(transform, Rotator);
		ImGui::SameLine();
		if (ImGui::DragFloat("Roll", &Rotator.z, 0.1f))
			Engine::TransformSystem::SetRotation(transform, Rotator);
		ImGui::PopID();


		// SCALE
		ImGui::PushID(id++);
		ImGui::Text("Scale");
		if (ImGui::DragFloat("X", &Scale.x, 0.01f))
			Engine::TransformSystem::SetScale(transform, Scale);
		ImGui::SameLine();
		if (ImGui::DragFloat("Y", &Scale.y, 0.01f))
			Engine::TransformSystem::SetScale(transform, Scale);
		ImGui::SameLine();
		if (ImGui::DragFloat("Z", &Scale.z, 0.01f))
			Engine::TransformSystem::SetScale(transform, Scale);
		ImGui::PopID();


		// RESET TRANSFORMS
		if (ImGui::Button("Reset Transformations"))
		{
			Location *= 0.f;
			Rotator *= 0.f;
			Scale = { 1,1,1 };
			Engine::TransformSystem::ResetTransforms(transform);
		}
	}
	void ImGuiSystem::GuiEntitySettings_Pathfinding(Scene* scene, int& id, Entity& m_Entity)
	{
		ImGui::PushItemWidth(150.f);
		ImGui::DragFloat("MovementSpeed", &m_Entity.GetComponent<Engine::PathfindingComponent>().m_EntityPathSpeed, 0.3f, 0.0f, 1000.f);


		// PATROL ---------------------------------------------------------------
		auto& pathfinder = m_Entity.GetComponent<Engine::PathfindingComponent>();
		auto& transform = m_Entity.GetComponent<Engine::TransformComponent>();
		glm::vec3 currentPos = transform.GetLocation() - glm::vec3(0, 0.5, 0);

		if (ImGui::Button("Start\nPatrol", ImVec2(100, 100)))
			Engine::PathfindingSystem::StartPatrol(scene, pathfinder, currentPos, pathfinder.m_PatrolType);

		ImGui::SameLine();
		if (ImGui::Button("Clear Patrol Path", ImVec2(90, 40))) {
			Engine::PathfindingSystem::ClearPatrol(pathfinder);
		}

		ImGui::SameLine();
		static char* charPatrolType{ "Loop" };
		if (ImGui::BeginCombo("Patrol Type", charPatrolType))
		{
			bool b{};
			if (ImGui::Selectable("Single", &b)) {
				pathfinder.m_PatrolType = Engine::PatrolType::Single;
				charPatrolType = "Single";
			}
			if (ImGui::Selectable("Loop", &b)) {
				pathfinder.m_PatrolType = Engine::PatrolType::Loop;
				charPatrolType = "Loop";
			}
			if (ImGui::Selectable("Reverse", &b)) {
				pathfinder.m_PatrolType = Engine::PatrolType::Reverse;
				charPatrolType = "Reverse";
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button(pathfinder.bPatrolling ? "Exit Patrol" : "Resume Patrol", ImVec2(100, 70)))
		{
			if (pathfinder.bPatrolling)
			{
				Engine::PathfindingSystem::PausePatrol(scene, pathfinder, currentPos);
			}
			else
			{
				Engine::PathfindingSystem::ResumePatrol(scene, pathfinder, currentPos, pathfinder.m_PatrolType);
			}
		}
#ifdef E_DEBUG
		ImGui::Checkbox("Show Path", &pathfinder.bRenderPath);
		ImGui::Checkbox("Show Patrol Points", &pathfinder.bRenderPatrolPoints);
#endif
	}
	void ImGuiSystem::GuiEntitySettings_Obstruction(Scene* scene, int& id, Entity& m_Entity)
	{
		ImGui::PushID(id++);
		glm::vec3& Location = m_Entity.GetComponent<Engine::TransformComponent>().GetLocation();
		auto& obs = m_Entity.GetComponent<Engine::ObstructionSphereComponent>();
		if (ImGui::DragFloat("Radius", &obs.m_radius, 0.1f, 0.0f, 100.f))
			Engine::NodeGridSystem::UpdateObstructionSphere(scene, obs.m_ID, obs.m_radius, Location);

		ImGui::PopID();
	}
}