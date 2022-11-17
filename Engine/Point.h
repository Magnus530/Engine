#pragma once

class Point
{
	std::vector <int> Neightbors;

public:
	void addneightbor(Point n) { Neightbors.push_back(n.id); }
	int id;


};

