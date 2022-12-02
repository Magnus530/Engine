#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Events/Event.h"
#include "glm/glm.hpp"

namespace Engine
{
	class ENGINE_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		//glm::vec3 RayCastFromScreenPosition(glm::vec2 screenPosition, glm::mat4 projection, glm::mat4 view, glm::vec2 windowSize = glm::vec2(1280, 720));

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};

}


