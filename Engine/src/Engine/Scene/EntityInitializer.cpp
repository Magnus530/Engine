#include "epch.h"
#include "EntityInitializer.h"
#include "Engine/AssetInit/PrimitiveShapeFactory.h"

namespace Engine
{
	EntityInitializer* EntityInitializer::m_Instance = new EntityInitializer;

	Engine::Entity EntityInitializer::EntityInit(const std::string objname, std::shared_ptr<Engine::Scene>& Scene)
	{
		Engine::Entity tempEntity = Scene->CreateEntity(objname);
		return tempEntity;
	}

	Engine::Entity EntityInitializer::EntityInit(const std::string objname, std::shared_ptr<Engine::VertexArray>& vertexarr, std::shared_ptr<Engine::Scene>& Scene)
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

		Engine::Entity tempEntity = Scene->CreateEntity(objname);

		return tempEntity;
	}

	Engine::Entity EntityInitializer::EntityInit(int shapenum, std::shared_ptr<Engine::VertexArray>& vertexarr, std::shared_ptr<Engine::Scene>& Scene)
	{
		vertexarr.reset(Engine::VertexArray::Create()); // OpenGLVertexArray*

		std::shared_ptr<Engine::VertexBuffer> PrimitiveVB;
		if (shapenum != 4)
		{
			std::vector<float> fVertices = Engine::PrimitiveShapeFactory::CreatePrimitiveShape(shapenum)->GetVertices();
			PrimitiveVB.reset(Engine::VertexBuffer::Create(fVertices.data(), fVertices.size() * sizeof(float))); // OpenGLVertexBuffer*	// for en vector av floats
			PrimitiveVB->SetLayout
			({
				{ Engine::ShaderDataType::Float3, "a_Position" },
				{ Engine::ShaderDataType::Float2, "a_TexCoord" }
			});
		}
		else
		{
			std::vector<Engine::Vertex> vVertices = Engine::PrimitiveShapeFactory::CreatePrimitiveShape(shapenum)->GetVVertices();
			PrimitiveVB.reset(Engine::VertexBuffer::Create(vVertices.data(), vVertices.size() * sizeof(Engine::Vertex))); // OpenGLVertexBuffer*	// for en vector av floats
			PrimitiveVB->SetLayout
			({
				{ Engine::ShaderDataType::Float3, "a_Position" },
				{ Engine::ShaderDataType::Float3, "a_Normal" },
				{ Engine::ShaderDataType::Float2, "a_TexCoord" }
			});
		}
		vertexarr->AddVertexBuffer(PrimitiveVB);

		std::vector<uint32_t> indices = Engine::PrimitiveShapeFactory::CreatePrimitiveShape(shapenum)->GetIndices();
		std::shared_ptr<Engine::IndexBuffer> PrimitiveIB;
		PrimitiveIB.reset(Engine::IndexBuffer::Create(indices)); // OpenGLIndexBuffer*
		vertexarr->SetIndexBuffer(PrimitiveIB);

		Engine::Entity tempEntity = Scene->CreateEntity("");

		return tempEntity;
	}
}