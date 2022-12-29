#pragma once

#include "Engine/Scene/Scene.h"

namespace Engine
{
	class ImGuiSystem
	{
	public:
		static void GuiEntitySettings(std::shared_ptr<Engine::Scene>& scene);
		static const char* EntityNameCompare(const std::string& name, std::vector<std::string>& names);
		static void ShaderSelect(std::shared_ptr<Engine::Entity>& entity, int& currShader, const char* comboPreview, std::vector<std::string>& names);
	};
}