#include <string>
#include <fstream>
#include <iostream>
 
#include "epch.h"
#include "Terrain.h"
#include "Triangle.h"
#include "Point.h"
#include "../Objects/vertex.h"

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

			Engine::Vertex v = Engine::Vertex(x, y, z);
			Point p = Point(Point_index, v);
			Point_index++;
			P.push_back(&p);
		}
		inn.close();
	}

	int looker{ 0 };

	bool is_at_end{ false };
	while (!is_at_end) {
		if (looker == Point_index)
			is_at_end = true;

	}

	Point *p1, *p2, *p3, *p4;
	float xMax{ 0 }, xMin{ 0 }, zMax{ 0 }, zMin{ 0 };
	for (int i = 0; i < P.size(); i++) {
		if (P[i]->getx() <= xMin && P[i]->getz() <= zMin) {
			xMin = P[i]->getx();
			zMin = P[i]->getz();
			p1 = P[i];
		}
		if (P[i]->getx() <= xMin && P[i]->getz() >= zMax) {
			xMin = P[i]->getx();
			zMax = P[i]->getz();
			p2 = P[i];
		}
		if (P[i]->getx() >= xMax && P[i]->getz() <= zMin) {
			xMax = P[i]->getx();
			zMin = P[i]->getz();
			p3 = P[i];
		}
		if (P[i]->getx() >= xMax && P[i]->getz() >= zMax) {
			xMax = P[i]->getx();
			zMax = P[i]->getz();
			p4 = P[i];
		}
	}

	Triangle t1 = Triangle(Triangle_index, p1, p2, p3);
	Triangle_index++;
	Triangle t2 = Triangle(Triangle_index, p2, p3, p4);
	Triangle_index++;

	p1->addneightbor(*p2);
	p1->addneightbor(*p3);

	p2->addneightbor(*p1);
	p2->addneightbor(*p3);
	p2->addneightbor(*p4);

	p3->addneightbor(*p1);
	p3->addneightbor(*p2);
	p3->addneightbor(*p4);

	p4->addneightbor(*p2);
	p4->addneightbor(*p3);

	T.push_back(&t1);
	T.push_back(&t2);
}