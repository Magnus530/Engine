#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/FMOD/AudioEngine.h"

namespace Engine
{
	class ImGuiSystem
	{
	public:
		void LayerSelection(class Application* app, std::vector<std::string>& layernames);
		void GuiEntitySettings(std::shared_ptr<Engine::Scene>& scene);
		void GuiPathfindingGridSettings(std::shared_ptr<Engine::Scene>& scene);
		const char* EntityNameCompare(const std::string& name, std::vector<std::string>& names);
		void ShaderSelect(std::shared_ptr<Engine::Entity>& entity, int& currShader, const char* comboPreview, std::vector<std::string>& names);
		void GuiAudioSettings(std::shared_ptr<Engine::AudioEngine>& audio);
	private:
		void GuiEntitySettings_Transform	(Scene* scene, int& id, Entity& m_Entity);
		void GuiEntitySettings_Pathfinding	(Scene* scene, int& id, Entity& m_Entity);
		void GuiEntitySettings_Obstruction	(Scene* scene, int& id, Entity& m_Entity);
	};
}