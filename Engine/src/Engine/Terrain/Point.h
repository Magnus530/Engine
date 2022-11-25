#pragma once
#include <vector>

#include <math.h>
#include "vertex.h"

class Point
{
public:
	Point() {};
	Point(int id, vertex v) { _id = id; info = v; };
	~Point() {};

	void addneightbor(Point n);
	void removeneighbor(Point n);

	float returndistance(Point n);
	float getx() { info.getX(); };
	float gety() { info.getY(); };
	float getz() { info.getZ(); };

	int _id;

private:
	std::vector <int> Neightbors;
	std::vector <float[2]> distances;
	vertex info;
};