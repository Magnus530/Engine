#pragma once
#include <iostream>

class vertex
{
	friend std::ostream& operator << (std::ostream&, const vertex&);
	friend std::istream& operator >> (std::istream&, vertex&);
public:
	vertex();
	vertex(float x, float y, float z, float r = 1, float g = 1, float b = 1, float u = 1, float v = 1);
	~vertex();

	float getX();
	float getY();
	float getZ();

	void setColor(float r, float g, float b);
private:
	float xyz[3];
	float normals[3];
	float uv[2];
};

