#include "epch.h"
#include "ImGuiSystem.h"
#include "Engine.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	void ImGuiSystem::GuiEntitySettings(std::shared_ptr<Engine::Scene>& scene)
	{
		ImGui::Begin("Entity Settings");

		int i = 0;
		std::vector<std::string> textureNames;
		std::vector<std::string> skyboxNames;

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
				static int currentTexture = 0;
				const char* comboTexPreviewValue = EntityNameCompare((it)->second->GetComponent<Engine::TextureMaterialComponent>().m_Tex.first, textureNames);

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
				ImGui::Separator();
				ImGui::PopID();
				i++;
				break;
			}
			case Engine::ShaderType::Phong:
			{		
				static int currentPhongTexture = 0;
				const char* comboPhongPreviewValue = EntityNameCompare((it)->second->GetComponent<Engine::PhongMaterialComponent>().m_Tex.first, textureNames);

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
				ImGui::Separator();
				ImGui::PopID();
				i++;
				break;

			}
			}
		}

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
}