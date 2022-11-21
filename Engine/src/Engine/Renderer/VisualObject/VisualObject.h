#pragma once

#include "vertex.h"
#include <vector>
#include "../VertexArray.h"
#include "../Buffer.h"


namespace Engine {

	class VisualObject
	{
	public:
		VisualObject();
		VisualObject(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
		~VisualObject();
		void Init(std::shared_ptr<VertexArray>& vertexarray);

		// Get functions
		glm::mat4 GetMatrix() const { return m_Matrix; }
		std::shared_ptr<VertexArray> GetVertexArray() const { return m_VA; }
		std::vector<uint32_t> GetIndices() const { return m_Indices; }

	private:
		glm::mat4 m_Matrix{ 1 };

		glm::mat4 m_Position{ 1 };
		glm::mat4 m_Rotation{ 1 };
		glm::mat4 m_Scale{ 1 };

		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		// Tmp method: each object holds their own vertex array and buffer pointers. 
		// TODO: Place these in the Renderer class for better handling
		std::shared_ptr<VertexArray> m_VA;
		std::shared_ptr<VertexBuffer> m_VB;
		std::shared_ptr<IndexBuffer> m_IB;
	};
}