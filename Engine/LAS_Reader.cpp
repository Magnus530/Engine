#include <string>
#include <fstream>
#include <iostream>
 
#include "epch.h"
#include "Terrain.h"
#include "Triangle.h"
#include "Point.h"
#include "vertex.h"

//#include "Readers.h"

std::vector <Triangle> LAS_Read(std::string filepath) {
	int Triangle_index{ 0 };
	int Point_index{ 0 };
	float x{ 0 }, y{ 0 }, z{ 0 };

	std::vector <Point*> P;
	std::vector <Triangle*> T;

	std::ifstream inn;
	inn.open(filepath.c_str());
	if (inn.is_open())
	{
		int n;
		inn >> n;
		
		for (int i = 0; i < n; i++) {
			inn >> x;
			inn >> y;
			inn >> z;

			vertex v = vertex(x, y, z);
			Point p = Point(Point_index, v);
			Point_index++;
			P.push_back(&p);
		}
		inn.close();
	}

	bool is_at_end{ false };
	int looker{ 0 };
	while (is_at_end) {
		if (looker == Point_index)
			is_at_end = true;
		Point p = *P[looker];
		p.addneightbor(*P[looker + 1]);
		p.addneightbor(*P[looker + 2]);
		p.addneightbor(*P[looker + 3]);

	}

}

void LAS_for_terrain() {

}