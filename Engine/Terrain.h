#pragma once

#include <vector>
#include <string>

class Terrain
{
public:
	Terrain();
	Terrain(std::string s);
	~Terrain();
private:
	std::vector <std::vector <float[3]>> vertices;
	std::vector <std::vector <float[3]>> colors;
	std::vector <std::vector <float[2]>> uv;
};

