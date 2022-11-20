#pragma once

#include "vertex.h"
#include <vector>
#include "vShader.h"
//#include "../Shader.h"


namespace Engine {

	class VisualObject
	{
	public:
		VisualObject();
		VisualObject(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
		~VisualObject();
	
		glm::mat4 GetMatrix() const { return m_Matrix; }

		void Init();
		void Draw();
		void Draw(glm::mat4& projectionMatrix, glm::mat4& viewMatrix);

		void SetShader(vShader* shader) { m_Shader = shader; }
	private:
		glm::mat4 m_Matrix{ 1 };

		glm::mat4 m_Position{ 1 };
		glm::mat4 m_Rotation{ 1 };
		glm::mat4 m_Scale{ 1 };

		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		uint32_t m_VAO{};
		uint32_t m_VBO{};
		uint32_t m_EAB{};

		vShader* m_Shader{ nullptr };
	};
}