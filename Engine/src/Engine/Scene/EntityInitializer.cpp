#include "epch.h"
#include "EntityInitializer.h"
#include "Engine/AssetInit/PrimitiveShapeFactory.h"
#include "Engine/Scene/Components.h"
#include "Platform/OpenGL/OpenGLCubemap.h"

namespace Engine
{
	EntityInitializer* EntityInitializer::m_Instance = new EntityInitializer;

	Engine::Entity EntityInitializer::EntityInit(const Engine::ShaderType& shaderType, std::string objname, std::shared_ptr<Engine::VertexArray>& vertexarr, 
		std::shared_ptr<Engine::Scene>& scene, const bool& isBBoard, const glm::vec3& color, std::pair<std::string, std::shared_ptr<Engine::Texture2D>> tex)
	{
		std::vector<Engine::Vertex> vertices;
		std::vector<uint32_t> indices;

		Engine::ObjLoader::ReadFile(objname, vertices, indices);
		vertexarr.reset(Engine::VertexArray::Create());
		std::shared_ptr<Engine::VertexBuffer> ObjVB;
		ObjVB.reset(Engine::VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Engine::Vertex))); // OpenGLVertexBuffer*	// for en vector av floats

		ObjVB->SetLayout
		({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float3, "a_Normal" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" }
		});
		vertexarr->AddVertexBuffer(ObjVB);

		std::shared_ptr<Engine::IndexBuffer> ObjIB;
		ObjIB.reset(Engine::IndexBuffer::Create(indices)); // OpenGLIndexBuffer*
		vertexarr->SetIndexBuffer(ObjIB);

		int i = 0;
		for (auto& it = scene->m_Entities.begin(); it != scene->m_Entities.end(); it++)
		{
			if (it->second->GetComponent<Engine::TagComponent>().Tag == objname)
			{
				objname += std::to_string(i);
			}
			i++;
		}
		Engine::Entity tempEntity = scene->CreateEntity(objname);
		tempEntity.AddComponent<RendererComponent>(vertexarr);
		MaterialInit(shaderType, tempEntity, isBBoard, color, tex);

		std::shared_ptr<Engine::Entity> sharedEntity = std::make_shared<Engine::Entity>(tempEntity);
		scene->m_Entities.insert({ objname, sharedEntity });

		return tempEntity;
	}

	Engine::Entity EntityInitializer::EntityInit(const std::string objname, std::shared_ptr<Engine::VertexArray>& va, 
		std::shared_ptr<Engine::Scene>& scene, std::pair<std::string, std::shared_ptr<Engine::OpenGLCubemap>> cubetex)
	{
		std::vector<Engine::Vertex> vertices;
		std::vector<uint32_t> indices;

		Engine::ObjLoader::ReadFile(objname, vertices, indices);
		va.reset(Engine::VertexArray::Create());
		std::shared_ptr<Engine::VertexBuffer> ObjVB;
		ObjVB.reset(Engine::VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Engine::Vertex))); // OpenGLVertexBuffer*	// for en vector av floats
		ObjVB->SetLayout
		({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float3, "a_Normal" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" }
			});
		va->AddVertexBuffer(ObjVB);

		std::shared_ptr<Engine::IndexBuffer> ObjIB;
		ObjIB.reset(Engine::IndexBuffer::Create(indices)); // OpenGLIndexBuffer*
		va->SetIndexBuffer(ObjIB);

		Engine::Entity tempEntity = scene->CreateEntity(objname);
		tempEntity.AddComponent<RendererComponent>(va);
		MaterialInit(tempEntity, cubetex);

		std::shared_ptr<Engine::Entity> sharedEntity = std::make_shared<Engine::Entity>(tempEntity);
		scene->m_Entities.insert({ objname, sharedEntity });

		return tempEntity;
	}

	Engine::Entity EntityInitializer::EntityInit(int shapenum, std::shared_ptr<Engine::VertexArray>& vertexarr, std::shared_ptr<Engine::Scene>& scene)
	{
		vertexarr.reset(Engine::VertexArray::Create()); // OpenGLVertexArray*

		std::shared_ptr<Engine::VertexBuffer> PrimitiveVB;
		std::vector<float> vertices = Engine::PrimitiveShapeFactory::CreatePrimitiveShape(shapenum)->GetVertices();
		PrimitiveVB.reset(Engine::VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float))); // OpenGLVertexBuffer*	// for en vector av floats
		PrimitiveVB->SetLayout
		({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float3, "a_Normal" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" }
		});
		vertexarr->AddVertexBuffer(PrimitiveVB);

		std::vector<uint32_t> indices = Engine::PrimitiveShapeFactory::CreatePrimitiveShape(shapenum)->GetIndices();
		std::shared_ptr<Engine::IndexBuffer> PrimitiveIB;
		PrimitiveIB.reset(Engine::IndexBuffer::Create(indices)); // OpenGLIndexBuffer*
		vertexarr->SetIndexBuffer(PrimitiveIB);

		Engine::Entity tempEntity = scene->CreateEntity("");

		return tempEntity;
	}

	void EntityInitializer::MaterialInit(const Engine::ShaderType& shaderType, Engine::Entity& entity, const bool& isBBoard,
		const glm::vec3& color, std::pair<std::string, std::shared_ptr<Engine::Texture2D>> tex)
	{
		if (isBBoard == true)
		{
			entity.AddComponent<BillboardMaterialComponent>("sss");
		}

		switch (shaderType)
		{
			case Engine::ShaderType::Flat:
			{
				entity.AddComponent<MaterialComponent>(Engine::ShaderType::Flat);
				entity.AddComponent<FlatMaterialComponent>(glm::vec4{ color, 1.f });
				break;
			}
			case Engine::ShaderType::Texture:
			{
				entity.AddComponent<MaterialComponent>(Engine::ShaderType::Texture);
				entity.AddComponent<TextureMaterialComponent>(tex.first, tex.second);
				break;
			}
			case Engine::ShaderType::Phong:
			{
				entity.AddComponent<MaterialComponent>(Engine::ShaderType::Phong);
				entity.AddComponent<PhongMaterialComponent>(glm::vec4{ color, 1.f }, tex.first, tex.second);
				break;
			}
		}
	}

	void EntityInitializer::MaterialInit(Engine::Entity& entity, std::pair<std::string, std::shared_ptr<Engine::OpenGLCubemap>> cubetex)
	{
		entity.AddComponent<MaterialComponent>(Engine::ShaderType::Skybox);
		entity.AddComponent<SkyboxMaterialComponent>(cubetex.first, cubetex.second);
	}
}