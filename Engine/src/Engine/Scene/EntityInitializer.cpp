#include "epch.h"
#include "EntityInitializer.h"
#include "Engine/AssetInit/PrimitiveShapeFactory.h"
#include "Engine/Scene/Components.h"
#include "Platform/OpenGL/OpenGLCubemap.h"
#include "../Terrain/Terrain.h"
#include "Engine/Scene/Systems/TransformSystem.h"

namespace Engine
{
	EntityInitializer* EntityInitializer::m_Instance = new EntityInitializer;

	Engine::Entity EntityInitializer::EntityInit(const Engine::ShaderType& shaderType, std::string objname, std::shared_ptr<Engine::VertexArray>& vertexarr, 
		std::shared_ptr<Engine::Scene>& scene, const bool& isBBoard, const glm::vec3& color, std::pair<std::string, std::shared_ptr<Engine::Texture2D>> tex)
	{
		std::vector<Engine::Vertex> vertices;
		std::vector<uint32_t> indices;

		if (objname == "Terrain") {
			Terrain T;
			T.init(vertices, indices);
		}
		else
		{
			Engine::ObjLoader::ReadFile(objname, vertices, indices);
		}
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
			case Engine::ShaderType::Particle:
			{
				entity.AddComponent<MaterialComponent>(Engine::ShaderType::Particle);
				entity.AddComponent<ParticleMaterialComponent>(glm::vec4{ color, 1.f }, tex.first, tex.second);
				break;
			}
			case Engine::ShaderType::Terrain:
			{
				entity.AddComponent<MaterialComponent>(Engine::ShaderType::Terrain);
				entity.AddComponent<TerrainMaterialComponent>(glm::vec4{ color, 1.f });
				break;
			}
		}
	}

	void EntityInitializer::MaterialInit(Engine::Entity& entity, std::pair<std::string, std::shared_ptr<Engine::OpenGLCubemap>> cubetex)
	{
		entity.AddComponent<MaterialComponent>(Engine::ShaderType::Skybox);
		entity.AddComponent<SkyboxMaterialComponent>(cubetex.first, cubetex.second);
	}

	Entity EntityInitializer::ObstructorEntityInit(const ShaderType shaderType, const std::string objname, std::shared_ptr<VertexArray> va, float radius, uint32_t id, Scene* scene, glm::vec3 color, std::pair<std::string, std::shared_ptr<Engine::Texture2D>> tex)
	{
		Entity tempEntity = scene->CreateEntity(objname);
		tempEntity.AddComponent<Engine::RendererComponent>(va);
		MaterialInit(shaderType, tempEntity, 0, color, tex);

		tempEntity.AddComponent<ObstructionSphereComponent>(radius, id);

		std::shared_ptr<Entity> sharedEntity = std::make_shared<Entity>(tempEntity);
		scene->m_Entities.insert({ objname, sharedEntity });
		return tempEntity;
	}

	Entity EntityInitializer::ObstructorEntityInit(const ShaderType shaderType, const std::string objname, std::shared_ptr<VertexArray> va, float radius, uint32_t id, Scene* scene)
	{
		Entity tempEntity = scene->CreateEntity(objname);
		tempEntity.AddComponent<Engine::RendererComponent>(va);
		tempEntity.AddComponent<MaterialComponent>(Engine::ShaderType::Texture);
		//tempEntity.AddComponent<FlatMaterialComponent>(glm::vec4{ glm::vec3{.3, .2, .5}, 1.f});
		tempEntity.AddComponent<TextureMaterialComponent>();

		tempEntity.AddComponent<ObstructionSphereComponent>(radius, id);

		std::shared_ptr<Entity> sharedEntity = std::make_shared<Entity>(tempEntity);
		scene->m_Entities.insert({ objname, sharedEntity });
		return tempEntity;
	}


	void EntityInitializer::CreateForest(std::string objname, const float& trees, const glm::vec3& forestCenter, const float& extent, std::shared_ptr<Engine::Scene>& scene)
	{
		std::srand(time(0));
		uint32_t rNum = 0;

		std::shared_ptr<Engine::VertexArray> va;
		Engine::Entity tempEntity;

		glm::vec3 bottomLeft = glm::vec3{ forestCenter.x - extent, forestCenter.y, forestCenter.z + extent };
		glm::vec3 bottomRight = glm::vec3{ forestCenter.x + extent, forestCenter.y, forestCenter.z + extent };
		glm::vec3 topLeft = glm::vec3{ forestCenter.x - extent, forestCenter.y, forestCenter.z - extent };
		glm::vec3 topRight = glm::vec3{ forestCenter.x + extent, forestCenter.y, forestCenter.z - extent };

		float rX = 0;
		float rZ = 0;
		glm::vec3 rPos{0};
		std::vector<std::shared_ptr<Engine::Entity>> treeVec;
		std::vector<std::shared_ptr<Engine::Entity>> treeCircleVec;
		float radius = extent / 2;

		for (int i = 0; i < trees; i++)
		{
			rNum = (1 + (rand() % 4));
			rX = (topLeft.x + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(topRight.x - topLeft.x))));
			rZ = (topLeft.z + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (bottomRight.z - topLeft.z))));

			//E_TRACE("rand num: {0}", rNum);

			switch (rNum)
			{
			case 1:
			{
				tempEntity = EntityInit(Engine::ShaderType::Phong, objname, va, scene, 1, glm::vec3{ 1.f, 1.f, 1.f }, *scene->m_Textures.find("Pine"));
				break;
			}
			case 2:
			{
				tempEntity = EntityInit(Engine::ShaderType::Phong, objname, va, scene, 1, glm::vec3{ 1.f, 1.f, 1.f }, *scene->m_Textures.find("Tree"));
				break;
			}
			case 3:
			{
				tempEntity = EntityInit(Engine::ShaderType::Phong, objname, va, scene, 1, glm::vec3{ 1.f, 1.f, 1.f }, *scene->m_Textures.find("Cypress"));
				break;
			}
			case 4:
			{
				tempEntity = EntityInit(Engine::ShaderType::Phong, objname, va, scene, 1, glm::vec3{ 1.f, 1.f, 1.f }, *scene->m_Textures.find("Fir"));
				break;
			}
			}

			rPos = glm::vec3{ rX, 2.7f, rZ };
			Engine::TransformSystem::SetWorldPosition(tempEntity.GetComponent<Engine::TransformComponent>(), rPos);
			Engine::TransformSystem::SetScale(tempEntity.GetComponent<Engine::TransformComponent>(), glm::vec3{ 2.f, 4.0f, 0.f });
			std::shared_ptr<Entity> sharedEntity = std::make_shared<Entity>(tempEntity);
			treeVec.push_back(sharedEntity);
		}

		for (int i = 0; i < treeVec.size(); i++)
		{
			auto& tempTransform = treeVec[i]->GetComponent<Engine::TransformComponent>();
			if (glm::distance(tempTransform.GetLocation(), forestCenter) <= radius && treeCircleVec.size() < radius)
			{
				treeCircleVec.push_back(treeVec[i]);
			}
			else
			{
				glm::vec3 tempDirVec = tempTransform.GetLocation() - forestCenter;
				glm::normalize(tempDirVec);
				tempDirVec * radius;
				Engine::TransformSystem::SetWorldPosition(tempTransform, tempDirVec);
			}
		}
	}

	//std::shared_ptr<VertexArray> EntityInitializer::VertexArrayInit(const std::string obj)
	//{
	//	std::vector<Vertex> vertices;
	//	std::vector<uint32_t> indices;
	//	Engine::ObjLoader::ReadFile(obj, vertices, indices);

	//	std::shared_ptr<VertexArray> va;
	//	va.reset(Engine::VertexArray::Create());
	//	std::shared_ptr<Engine::VertexBuffer> ObjVB;
	//	ObjVB.reset(Engine::VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Engine::Vertex))); // OpenGLVertexBuffer*	// for en vector av floats
	//	ObjVB->SetLayout
	//	({
	//		{ Engine::ShaderDataType::Float3, "a_Position" },
	//		{ Engine::ShaderDataType::Float3, "a_Normal" },
	//		{ Engine::ShaderDataType::Float2, "a_TexCoord" }
	//		});
	//	va->AddVertexBuffer(ObjVB);

	//	std::shared_ptr<Engine::IndexBuffer> ObjIB;
	//	ObjIB.reset(Engine::IndexBuffer::Create(indices)); // OpenGLIndexBuffer*
	//	va->SetIndexBuffer(ObjIB);
	//	return va;
	//}
}