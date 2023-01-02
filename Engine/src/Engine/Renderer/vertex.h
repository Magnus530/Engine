#pragma once
#include <iostream>
#include <glm/glm.hpp>

namespace Engine {

	class Vertex
	{
		friend std::ostream& operator << (std::ostream&, const Vertex&);
		friend std::istream& operator >> (std::istream&, Vertex&);

	private:
		float m_xyz[3];
		float m_normals[3];
		float m_uv[2];

	public:
		Vertex();
		Vertex(glm::vec3 position);
		Vertex(glm::vec3 position, glm::vec3 normal);
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv);
		Vertex(float x, float y, float z, float r = 1, float g = 1, float b = 1, float u = 1, float v = 1);
		~Vertex();

		void SetPosition(float x, float y, float z);
		void SetNormal(float r, float g, float b);
		void SetNormal(glm::vec3 normal);
		void SetUV(glm::vec2 uv);

		glm::vec3 GetPosition() const;
		glm::vec3 GetNormal() const;
		glm::vec2 GetUv() const;

		float GetX() const { return m_xyz[0]; }
		float GetY() const { return m_xyz[1]; }
		float GetZ() const { return m_xyz[2]; }
		float GetR() const { return m_normals[0]; }
		float GetG() const { return m_normals[1]; }
		float GetB() const { return m_normals[2]; }
		float GetU() const { return m_uv[0]; }
		float GetV() const { return m_uv[1]; }
	};


}