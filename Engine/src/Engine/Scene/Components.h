#pragma once

#include "Engine/Renderer/Buffer.h"
#include "Engine/Renderer/VertexArray.h"

#include <glm/glm.hpp>

namespace Engine
{
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::mat4 m_Transform{ 1.0f };
		glm::mat4 m_Position{ 1.0f };
		glm::mat4 m_Rotation{ 1.0f };
		glm::mat4 m_Scale{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: m_Transform(transform) {}

		operator glm::mat4& () { return m_Transform; }
		operator const glm::mat4& () const { return m_Transform; }
	};

	struct RendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };


		std::shared_ptr<VertexArray> m_VA;
		std::shared_ptr<VertexBuffer> m_VB;
		std::shared_ptr<IndexBuffer> m_IB;

		RendererComponent() = default;
		RendererComponent(const RendererComponent&) = default;
		RendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct PathfindingComponent
	{
		PathfindingComponent() = default;
		PathfindingComponent(const PathfindingComponent&) = default;
	};

}