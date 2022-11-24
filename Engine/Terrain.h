#pragma once

#include <vector>
#include <string>

#include "Triangle.h"

class Terrain
{
public:
	Terrain() {};
	Terrain(std::vector <Triangle*> &T) { Triangles = T; };
	~Terrain() {};

private:
	std::vector <Triangle*> Triangles;
};