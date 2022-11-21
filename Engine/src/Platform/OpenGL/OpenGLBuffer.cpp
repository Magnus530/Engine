#include "epch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Engine
{
	/* ------------------------------------ VertexBuffer ------------------------------------ */

	//OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	//{
	//	glCreateBuffers(1, &m_RendererID);
	//	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	//	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	//}

	OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, uint32_t size)
	{
		//glCreateBuffers(1, &m_RendererID);
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}	
	OpenGLVertexBuffer::OpenGLVertexBuffer(std::vector<Vertex>& vertices)
	{
		//glCreateBuffers(1, &m_RendererID);
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, (uint32_t)vertices.data()*sizeof(Vertex), (void*)vertices.size(), GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_VBO);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	/* ------------------------------------ IndexBuffer ------------------------------------ */

	//OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
	//OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, unsigned int size)
	OpenGLIndexBuffer::OpenGLIndexBuffer(std::vector<uint32_t>& indices)
		: m_Count(indices.size())
	{
		glCreateBuffers(1, &m_EAB);
		//glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EAB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_EAB);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EAB);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}