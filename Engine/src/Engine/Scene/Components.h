#pragma once

#include "Engine/Renderer/Buffer.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/PerspectiveCameraController.h"
#include "Engine/Renderer/Shader.h"
#include "Platform/OpenGL/OpenGLCubemap.h"

#include <utility>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Engine/Particles/basicparticlemanager.h"
# define M_PI           3.14159265358979323846 


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
		glm::vec3 m_Rotator{ 0.f };	// Temporary method

		glm::vec3 GetLocation() const { return m_Transform[3]; }
		glm::vec3 GetScale() const
		{
			float X = glm::length(m_Transform[0]);
			float Y = glm::length(m_Transform[1]);
			float Z = glm::length(m_Transform[2]);
			return { X, Y, Z };
		}
		glm::vec3 GetRotator()
		{
			float X = (float)std::atan2(m_Transform[1].z, m_Transform[2].z);
			float Y = (float)std::atan2(-m_Transform[0].z, sqrt(pow(m_Transform[1].z,2) + pow(m_Transform[2].z,2)));
			float Z = (float)std::atan2(m_Transform[0].y, m_Transform[0].x);

			return { X,Y,Z };
		}

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		operator glm::mat4& () { return m_Transform; }
		operator const glm::mat4& () const { return m_Transform; }
	};

	struct RendererComponent
	{
		std::shared_ptr<VertexArray> m_VA;

		bool m_bCustomColor = false;

		RendererComponent() = default;
		RendererComponent(const RendererComponent&) = default;
		RendererComponent(std::shared_ptr<VertexArray> va)
			: m_VA(va) {}
	};

	struct MaterialComponent
	{
		Engine::ShaderType m_ShaderType;
		MaterialComponent(const ShaderType& shaderType)
			: m_ShaderType(shaderType) {}
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

		std::pair<std::string, std::shared_ptr<Texture2D>> m_Tex;

		TextureMaterialComponent() = default;
		TextureMaterialComponent(const TextureMaterialComponent&) = default;
		TextureMaterialComponent(const std::string name, const std::shared_ptr<Texture2D>& texture)
		{
			m_Tex.first = name;
			m_Tex.second = texture;
		}
	};

	struct PhongMaterialComponent
	{
		glm::vec4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		std::pair<std::string, std::shared_ptr<Texture2D>> m_Tex;
		glm::vec3 m_PCamPosition{ 0.f, 0.f, 0.f};

		float m_AmbientStrength{ 1.f };
		glm::vec3 m_AmbientColor{ m_Color.x, m_Color.y, m_Color.z };
		glm::vec3 m_LightPosition{ 0.f, 0.f, 0.f }; 
		glm::vec3 m_LightColor{ 0.9f, 0.9f, 0.3f };
		float m_SpecularStrength{ 3.f };

		PhongMaterialComponent() = default;
		PhongMaterialComponent(const PhongMaterialComponent&) = default;
		PhongMaterialComponent(const glm::vec4& color, const std::string name, const std::shared_ptr<Texture2D>& texture)
			: m_Color(color) 
		{
			m_Tex.first = name;
			m_Tex.second = texture;
		}
	};

	struct ParticleMaterialComponent
	{
		std::pair<std::string, std::shared_ptr<Texture2D>> m_Tex;
		glm::vec4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		ParticleMaterialComponent() = default;
		ParticleMaterialComponent(const ParticleMaterialComponent&) = default;
		ParticleMaterialComponent(const glm::vec4& color, const std::string name, const std::shared_ptr<Texture2D>& texture)
			: m_Color(color)
		{
			m_Tex.first = name;
			m_Tex.second = texture;
		}
	};

	struct TerrainMaterialComponent
	{
		glm::vec4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		TerrainMaterialComponent() = default;
		TerrainMaterialComponent(const TerrainMaterialComponent&) = default;
		TerrainMaterialComponent(const glm::vec4& color)
			: m_Color(color) {}
	};

	struct SkyboxMaterialComponent
	{
		std::pair<std::string, std::shared_ptr<Engine::OpenGLCubemap>> m_CubeTex;

		SkyboxMaterialComponent() = default;
		SkyboxMaterialComponent(const SkyboxMaterialComponent&) = default;
		SkyboxMaterialComponent(const std::string name, const std::shared_ptr<Engine::OpenGLCubemap>& texture)
		{
			m_CubeTex.first = name;
			m_CubeTex.second = texture;
		}
	};

	struct BillboardMaterialComponent
	{
		std::string s;

		BillboardMaterialComponent() = default;
		BillboardMaterialComponent(const BillboardMaterialComponent&) = default;
		BillboardMaterialComponent(const std::string& ss)
			: s(ss) {}
	};

	struct LightComponent
	{
		float m_AmbientStrength{ 1.f };
		glm::vec3 m_AmbientColor{ 0.9f, 0.9f, 0.3f };
		float m_LightStrength{ 0.5f };
		glm::vec3 m_LightColor{ 0.9f, 0.9f, 0.3f};
		float m_SpecularStrength{ 3.f };
		int m_SpecularExponent{ 0 };

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(const float lightStrength, const float specularStrength)
			: m_LightStrength(lightStrength), m_SpecularStrength(specularStrength) {}
	};

		enum PatrolType
		{
			Single,
			Loop,
			Reverse,
		};
	struct PathfindingComponent
	{
		int m_StartNode{};
		int m_TargetNode{};
		int m_IntermediateTargetNode{};

		// Moving Through Path
		std::vector<int> m_CurrentPath;	
		std::shared_ptr<struct BSpline> m_SplinePath = std::make_shared<BSpline>();
		float m_SplineMovement{};	// How far along the spline the Entity has gone
		bool bIsMovingAlongPath{};
		bool bReachedTarget{};
		bool bIsObstructed{};

		// MovementSpeed
		float m_SplineLength{};
		float m_EntityPathSpeed{ 5.f };

		// Patrol
		bool bPatrolling{};
		bool bReverse{};	// For PatrolType::Reverse
		PatrolType m_PatrolType = PatrolType::Loop;
		std::vector<glm::vec3> m_PatrolPath;
		int m_CurrentPatrolPoint{};
		glm::vec3 m_PatrolPauseLocation;
#ifdef E_DEBUG
		bool bRenderPatrolPoints{};
#endif

		PathfindingComponent() = default;
		PathfindingComponent(const PathfindingComponent&) = default;
	};

	struct ObstructionSphereComponent
	{
		ObstructionSphereComponent(float radius, uint32_t id)
			: m_radius{radius}, m_ID{id}{}
		float m_radius{ 2.f };
		uint32_t m_ID{};
	};

	struct ParticleManagerComponent
	{
		particles::BasicParticleManager* m_ParticleManager;

		ParticleManagerComponent() = default;
		explicit ParticleManagerComponent(size_t maxCount, glm::vec4 loc)
		{
			m_ParticleManager = new particles::BasicParticleManager(maxCount, loc);
		}
		void update(double dt) {
			m_ParticleManager->update(dt);
		}
	};
}
