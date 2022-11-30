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
		glm::mat4 m_Transform	{ 1.f };
		glm::mat4 m_Position	{ 1.f };
		glm::mat4 m_Rotation	{ 1.f };
		glm::mat4 m_Scale		{ 1.f };
		float m_Speed{ 2.f };

		glm::vec3 GetPosition() const { return m_Transform[3]; }

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

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
		std::shared_ptr<class PNode> m_StartNode;
		std::shared_ptr<struct NodeGrid> m_Grid;
		std::shared_ptr<class PNode> m_TargetNode;
		std::shared_ptr<class PNode> m_IntermediateTargetNode;

		std::vector<std::shared_ptr<class PNode>> m_CurrentPath;	// reduntant TODO: remove
		std::shared_ptr<struct BSpline> m_SplinePath = std::make_shared<BSpline>();
		float m_SplineMovement{};	// How far along the spline has the Entity gone?
		bool bStartedPathfinding{};
		bool bReachedTarget{};
		bool bIsObstructed{};

		PathfindingComponent() = default;
		PathfindingComponent(const PathfindingComponent&) = default;
	};

	struct ObstructionSphereComponent
	{
		float m_radius{ 2.f };
		uint32_t m_ID{};
	};

	//class TestComponent
	//{
	//public:
	//	TestComponent();
	//	~TestComponent();

	//	int data{};
	//};
}