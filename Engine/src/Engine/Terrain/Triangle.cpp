#include <glm/glm.hpp>

#include "epch.h"
#include "Triangle.h"

#include <cmath>

//namespace Engine
//{
	Triangle::Triangle(int i1, int i2, int i3, std::vector <Engine::Vertex> v) {
		child = false;
		point[0] = i1;
		point[1] = i2;
		point[2] = i3;

		FindCenter(v[i1], v[i2], v[i3]);
		Circle_Rad(v[i3]);

		for (int i = 0; i < v.size(); i++) {
			if (!(i == i1 || i == i2 || i == i3)) {
				glm::vec2 D = { v[i].GetX(), v[i].GetZ() };
				glm::vec2 O = { origo.GetX(), origo.GetZ() };
				glm::vec2 OD = O - D;
				float Odlength = sqrt(((OD.x * OD.x) + (OD.y * OD.y)));
				if (Odlength < radious) {
					PotentialChildren.push_back(i);
					child = true;
				}
			}
		}
		if (i1 != 10 && i1 != 11 && i1 != 12) {
			if (i2 != 10 && i2 != 11 && i2 != 12) {
				if (i3 != 10 && i3 != 11 && i3 != 12) {
					bool c = child;
					int f = PotentialChildren.size();
					int k = 7;
				}
			}
		}
		int stop = 8;/**/
	}

	void Triangle::FindCenter(Engine::Vertex a, Engine::Vertex b, Engine::Vertex c) {
		float h = (a.GetY() + b.GetY() + c.GetY()) / 3;

		glm::vec2 A = { a.GetX(), a.GetZ() };
		glm::vec2 B = { b.GetX(), b.GetZ() };
		glm::vec2 C = { c.GetX(), c.GetZ() };

		float x, z;
		float D = (((B.y - C.y) * A.x) + ((C.y - A.y) * B.x) + ((A.y - B.y) * C.x)) * 2; //https://www.youtube.com/watch?v=4ySSsESzw2Y

		x = (1 / D) * ((((A.x * A.x) + (A.y * A.y)) * (B.y - C.y)) + (((B.x * B.x) + (B.y * B.y)) * (C.y - A.y)) + (((C.x * C.x) + (C.y * C.y)) * (A.y - B.y)));
		z = (1 / D) * ((((A.x * A.x) + (A.y * A.y)) * (C.x - B.x)) + (((B.x * B.x) + (B.y * B.y)) * (A.x - C.x)) + (((C.x * C.x) + (C.y * C.y)) * (B.x - A.x)));

		Engine::Vertex v = Engine::Vertex(x, h, z);
		origo = v;
	}

	void Triangle::Circle_Rad(Engine::Vertex a) {
		float r = sqrt(((((a.GetX() - origo.GetX()) * (a.GetX() - origo.GetX()))) + ((a.GetZ() - origo.GetZ()) * (a.GetZ() - origo.GetZ()))));
		bool isNumber = std::isfinite(r);
		if (isNumber) {
			radious = r;
		}
		else {
			radious = (-1);
		}
	}
//}