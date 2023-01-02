#pragma once

#include <vector>
#include <string>

#include "../Objects/vertex.h"

class Terrain
{
public:
	Terrain() {};
	Terrain(std::string filepath);
	~Terrain() {};

	void init();
	void draw();
private:
	std::vector <std::vector<int>> Triangles;
	std::vector <Engine::Vertex> mVertices;
	//std::vector <GLuint> mIndices;

	void CreateTriangles(int index_1, int index_2, int index_3);

	float TriangleRadious();

	Engine::Vertex CircleCenter();
};