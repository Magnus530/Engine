#include "epch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Engine
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Engine::ShaderDataType::Float:		return GL_FLOAT;
			case Engine::ShaderDataType::Float2:	return GL_FLOAT;
			case Engine::ShaderDataType::Float3:	return GL_FLOAT;
			case Engine::ShaderDataType::Float4:	return GL_FLOAT;
			case Engine::ShaderDataType::Mat3:		return GL_FLOAT;
			case Engine::ShaderDataType::Mat4:		return GL_FLOAT;
			case Engine::ShaderDataType::Int:		return GL_INT;
			case Engine::ShaderDataType::Int2:		return GL_INT;
			case Engine::ShaderDataType::Int3:		return GL_INT;
			case Engine::ShaderDataType::Int4:		return GL_INT;
			case Engine::ShaderDataType::Bool:		return GL_BOOL;
		}

		E_CORE_ASSERT(false, "Unknown ShaderDataType.");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glGenVertexArrays(1, &m_RendererID);
		//glCreateVertexArrays(1, &m_VAO);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		E_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer is missing a layout.");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				//(const void*)element.Offset);
				reinterpret_cast<GLvoid*>(0*index));
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::AddVertexBuffer(const uint32_t& vertexBuffer, uint64_t& data)
	{
		glBindVertexArray(m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, data, reinterpret_cast<void*>(0));// array buffer offset
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, data, reinterpret_cast<void*>(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, data, reinterpret_cast<void*>(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		//glGenBuffers(1, &m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}