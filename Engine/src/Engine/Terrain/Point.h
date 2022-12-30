#pragma once
#include <vector>
#include <math.h>

#include "../Objects/vertex.h"

class Point
{
public:
	Point() {};
	Point(int id, Engine::Vertex v) { _id = id; info = v; };
	~Point() {};

	void addneightbor(Point n);
	void removeneighbor(Point n);

	float returndistance(Point n);
	float getx() { info.GetX(); };
	float gety() { info.GetY(); };
	float getz() { info.GetZ(); };

	int _id;

private:
	std::vector <int> Neightbors;
	std::vector <float[2]> distances;
	Engine::Vertex info;
};