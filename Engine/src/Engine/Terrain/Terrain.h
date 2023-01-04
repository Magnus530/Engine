#pragma once

#include <vector>
#include <string>

#include "Triangle.h"
#include "../Renderer/vertex.h"

//namespace Engine
//{
	class Terrain
	{
	public:
		Terrain();
		~Terrain() {};

		void init(std::vector<Engine::Vertex>& vertices, std::vector<uint32_t>& indices);
		void draw();
	private:
		std::vector <Triangle> Triangles;
		std::vector <Engine::Vertex> mVertices;
		std::vector <uint32_t> mIndices;

		float to_float(std::string s);
		int to_int(std::string s);

		void CreateTriangles(int index_1, int index_2, int index_3);
		glm::vec3 bayercentricCoordinates(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d);
	};
//}