#pragma once

#include <memory>
#include "Engine/Renderer/Buffer.h"

#include "Engine/Renderer/vertex.h"
#include "Engine/AssetInit/ObjLoader.h"

namespace Engine
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
		virtual void AddVertexBuffer(const uint32_t& vertexBuffer, uint64_t& data) = 0;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;
		//virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;
		//virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;
		
		//static std::shared_ptr<VertexArray> Create();
		static VertexArray* Create();
	};

#ifdef E_DEBUG
	inline static void InitVertexArray(std::string obj, std::shared_ptr<VertexArray>& va)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		ObjLoader::ReadFile(obj, vertices, indices);
		va.reset(VertexArray::Create());
		std::shared_ptr<VertexBuffer> ObjVB;
		ObjVB.reset(VertexBuffer::Create(vertices.data(), (uint32_t)vertices.size() * sizeof(Vertex))); // OpenGLVertexBuffer*	// for en vector av floats
		ObjVB->SetLayout
		({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		va->AddVertexBuffer(ObjVB);

		std::shared_ptr<IndexBuffer> ObjIB;
		ObjIB.reset(IndexBuffer::Create(indices)); // OpenGLIndexBuffer*
		va->SetIndexBuffer(ObjIB);
	}
#endif
}