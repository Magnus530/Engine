#pragma once

#include "Engine/Renderer/RenderState.h"
#include "Engine/Core/Log.h"

namespace Engine
{
	class RenderContext
	{
	public:
		RenderContext(Engine::RenderState* state, Entity& entity, std::shared_ptr<Engine::ShaderLibrary> shaderLibrary, Engine::Renderer::SceneData* sceneData)
			: m_StatePtr(nullptr), m_Entity(entity), m_ShaderLibrary(shaderLibrary), m_SceneData(sceneData)
		{
			TransitionTo(state);
		}

		~RenderContext()
		{
			delete m_StatePtr;
		}

		void TransitionTo(Engine::RenderState* state)
		{
			//E_TRACE("RenderContext: Transition to {0}", typeid(state).name());
			if (m_StatePtr != nullptr)
			{
				delete m_StatePtr;
			}
			m_StatePtr = state;
			m_StatePtr->SetRenderContext(this);
		}

		void InitShader()
		{
			m_StatePtr->InitShader(m_Entity, m_ShaderLibrary, m_SceneData);
		}

	private:
		Engine::RenderState* m_StatePtr = nullptr;
		Entity& m_Entity;
		std::shared_ptr<Engine::ShaderLibrary> m_ShaderLibrary;
		Engine::Renderer::SceneData* m_SceneData = nullptr;
	};
}