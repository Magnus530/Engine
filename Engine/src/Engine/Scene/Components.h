#pragma once

#include "Engine/Renderer/Buffer.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Texture.h"

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

		glm::vec3 GetPosition() const { return m_Transform[3]; }

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		operator glm::mat4& () { return m_Transform; }
		operator const glm::mat4& () const { return m_Transform; }
	};

	struct RendererComponent
	{
		glm::vec4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		std::shared_ptr<VertexArray> m_VA;
		std::shared_ptr<VertexBuffer> m_VB;
		std::shared_ptr<IndexBuffer> m_IB;

		std::shared_ptr<Texture2D> m_Tex = nullptr;

		bool m_bCustomColor = false;

		RendererComponent() = default;
		RendererComponent(const RendererComponent&) = default;
		RendererComponent(const glm::vec4& color)
			: m_Color(color) {}
	};

		enum PatrolType
		{
			// In Order of m_PatrolPath
			Single,
			Loop,
			Reverse,
		};
	struct PathfindingComponent
	{
		// Start node and current grid
		std::shared_ptr<class PNode> m_StartNode;
		std::shared_ptr<struct NodeGrid> m_Grid;
		std::shared_ptr<class PNode> m_TargetNode;
		std::shared_ptr<class PNode> m_IntermediateTargetNode;

		// Moving Through Path
		std::vector<std::shared_ptr<class PNode>> m_CurrentPath;	// reduntant TODO: replace with glm::vec3 Positions
		std::shared_ptr<struct BSpline> m_SplinePath = std::make_shared<BSpline>();
		float m_SplineMovement{};	// How far along the spline has the Entity gone?
		bool bIsMovingAlongPath{};
		bool bReachedTarget{};
		bool bIsObstructed{};

		// MovementSpeed
		float m_SplineLength{};
		float m_EntityPathSpeed{ 5.f };

		// Patrol
		bool bPatrolling{};
			/* Only relevant for PatrolType::Reverse 
				Checks if entity is currently going the reverse way of the path */
		bool bReverse{};
		PatrolType m_PatrolType = PatrolType::Loop;
		std::vector<glm::vec3> m_PatrolPath;
		int m_CurrentPatrolPoint{};
		glm::vec3 m_PatrolPauseLocation;

		PathfindingComponent() = default;
		PathfindingComponent(const PathfindingComponent&) = default;
	};

	struct ObstructionSphereComponent
	{
		float m_radius{ 2.f };
		uint32_t m_ID{};
	};
}