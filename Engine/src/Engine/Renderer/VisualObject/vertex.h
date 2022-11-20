#pragma once
#include <iostream>
#include <glm/glm.hpp>

namespace Engine {

	class Vertex
	{
		friend std::ostream& operator << (std::ostream&, const Vertex&);
		friend std::istream& operator >> (std::istream&, Vertex&);

	public:
		Vertex();
		Vertex(glm::vec3 position);
		Vertex(glm::vec3 position, glm::vec3 normal);
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv);
		Vertex(float x, float y, float z, float r = 1, float g = 1, float b = 1, float u = 1, float v = 1);
		~Vertex();

		glm::vec3 GetPosition() const;
		glm::vec3 GetNormal() const;
		glm::vec2 GetUv() const;

		float GetX() const;
		float GetY() const;
		float GetZ() const;
		float GetR() const;
		float GetG() const;
		float GetB() const;

		void SetNormal(float r, float g, float b);
		void SetNormal(glm::vec3 normal);
	private:
		float Xyz[3];
		float Normals[3];
		float Uv[2];
	};


}