#pragma once

#include "Engine/Renderer/Buffer.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/PerspectiveCameraController.h"

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
		float m_Speed{ 2.f };

		glm::vec3 GetPosition() const { return m_Transform[3]; }

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		operator glm::mat4& () { return m_Transform; }
		operator const glm::mat4& () const { return m_Transform; }
	};

	struct RendererComponent
	{
		std::shared_ptr<VertexArray> m_VA;
		std::shared_ptr<VertexBuffer> m_VB;
		std::shared_ptr<IndexBuffer> m_IB;

		bool m_bCustomColor = false;

		RendererComponent() = default;
		RendererComponent(const RendererComponent&) = default;
		RendererComponent(std::shared_ptr<VertexArray> va)
			: m_VA(va) {}
	};

	struct FlatMaterialComponent
	{
		glm::vec4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		FlatMaterialComponent() = default;
		FlatMaterialComponent(const FlatMaterialComponent&) = default;
		FlatMaterialComponent(const glm::vec4& color)
			: m_Color(color) {}
	};

	struct TextureMaterialComponent
	{
		//uint8_t data[] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

		std::shared_ptr<Texture2D> m_Tex = nullptr;

		TextureMaterialComponent() = default;
		TextureMaterialComponent(const TextureMaterialComponent&) = default;
		TextureMaterialComponent(const std::shared_ptr<Texture2D>& texture)
			: m_Tex(texture) {}
	};

	struct PhongMaterialComponent
	{
		glm::vec4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		std::shared_ptr<Texture2D> m_Tex = nullptr;
		glm::vec3 m_PCamPoition{ 0.f, 0.f, 0.f};
		glm::vec3 m_LightColor{ 0.9f, 0.9f, 0.3f };
		float m_SpecularStrength{ 3.f };
		glm::vec3 m_LightPosition{1}; 

		PhongMaterialComponent() = default;
		PhongMaterialComponent(const PhongMaterialComponent&) = default;
		PhongMaterialComponent(const glm::vec4& color, const std::shared_ptr<Texture2D>& texture, glm::vec3& pCamPos,
			const glm::vec3& lightColor, const float& specularStrength, const glm::vec3& lightPosition)
			: m_Color(color), m_Tex(texture), m_PCamPoition(pCamPos), m_LightColor(lightColor), m_SpecularStrength(specularStrength), m_LightPosition(lightPosition) {}
	};

	struct LightComponent
	{
		float m_AmbientStrength{ 10.f };
		glm::vec3 m_AmbientColor{ 0.6, 0.6, 0.6 };
		float m_LightStrength{ 1.f };
		glm::vec3 m_LightColor{0.9f, 0.9f, 0.3f};
		float m_SpecularStrength{ 3.f };
		int m_SpecularExponent{ 4 };
		glm::vec3 m_ObjectColor{ 1.f, 1.f, 1.f };

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(const float lightStrength, const float specularStrength)
			: m_LightStrength(lightStrength), m_SpecularStrength(specularStrength) {}
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
}