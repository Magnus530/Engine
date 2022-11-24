#pragma once
#include <math.h>
#include "vertex.h"

class Point
{
public:
	Point() {};
	Point(int id, vertex v) { _id = id; info = v; };
	~Point() {};

	void addneightbor(Point n);

	float returndistance(Point n);

	int _id;

private:
	std::vector <int> Neightbors;
	std::vector <float[2]> distances;
	vertex info;
};