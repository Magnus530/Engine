#pragma once

#include "Engine/Renderer/Buffer.h"

namespace Engine
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		//OpenGLVertexBuffer(float* vertices, uint32_t size);
		OpenGLVertexBuffer(void* data, uint32_t size);
		OpenGLVertexBuffer(std::vector<Vertex>& vertices);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private:
		uint32_t m_VBO{};
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		//OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		//OpenGLIndexBuffer(uint32_t* indices, size_t size);
		OpenGLIndexBuffer(std::vector<uint32_t>& indices);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual uint32_t GetCount() const { return m_Count; }

	private:
		uint32_t m_EAB{};
		uint32_t m_Count{};
	};
}