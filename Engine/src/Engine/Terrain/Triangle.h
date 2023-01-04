#pragma once
#include <vector>

#include "../Renderer/vertex.h"

class Triangle
{
public:
	Triangle() {};
	Triangle(int index_1, int index_2, int index_3, std::vector <Engine::Vertex> vertices);
	~Triangle() {};

	Engine::Vertex center() const { return origo; };
	float rad() const { return radious; };
	bool havechild() const { return child; };

	std::vector <int> PotentialChildren;
	int point[3];
private:
	Engine::Vertex origo;
	float radious;
	bool child;

	void FindCenter(Engine::Vertex a, Engine::Vertex b, Engine::Vertex c);
	void Circle_Rad(Engine::Vertex a);
};

