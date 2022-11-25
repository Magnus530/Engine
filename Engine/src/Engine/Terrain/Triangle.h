#pragma once
#include "Point.h"
class Triangle
{
public:
	Triangle() {};
	Triangle(int id, Point *a, Point *b, Point *c) { _id = id; points[0] = a; points[1] = b; points[2] = c; };
	~Triangle() {};

	bool is_triangle(Triangle t);
	void change_triangle(Point *a, Point *b, Point *c) { points[0] = a; points[1] = b; points[2] = c; };
	float circle_rad();

	int _id;
	vertex center;
private:
	float Areal();

	Point* points[3];
};