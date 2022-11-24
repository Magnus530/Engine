#include "epch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Engine
{
	/* ------------------------------------ VertexBuffer ------------------------------------ */
	OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, uint32_t size)
	{
		glCreateBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}	
	//OpenGLVertexBuffer::OpenGLVertexBuffer(std::vector<Vertex>& vertices)
	//{
	//	glCreateBuffers(1, &m_VBO);
	//	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	//	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	//}

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
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indicesdata, uint32_t count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_EAB);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EAB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), indicesdata, GL_STATIC_DRAW);
	}
	OpenGLIndexBuffer::OpenGLIndexBuffer(std::vector<uint32_t>& indices)
		: m_Count(indices.size())
	{
		glCreateBuffers(1, &m_EAB);
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