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
		Circle_Rad(v[i1]);

		for (int i = 0; i < v.size(); i++) {
			if (!(i == point[0] || i == point[1] || i == point[2])) {
				glm::vec2 D = { v[i].GetX(), v[i].GetZ() };
				glm::vec2 O = { origo.GetX(), origo.GetZ() };
				glm::vec2 OD = O - D;
				if (OD.length() < radious) {
					PotentialChildren.push_back(i);
				}
			}
		}
		if (PotentialChildren.size() > 0) {
			child = true;
		}
	}

	void Triangle::FindCenter(Engine::Vertex a, Engine::Vertex b, Engine::Vertex c) {
		float h = (a.GetY() + b.GetY() + c.GetY()) / 3;

		float x, z;
		float D = ((b.GetZ() - c.GetZ()) * a.GetX() + (c.GetZ() - a.GetZ()) * b.GetX() + (a.GetZ() - b.GetZ()) * c.GetX()) * 2; //https://www.youtube.com/watch?v=4ySSsESzw2Y

		x = (1 / D) * (((a.GetX() * a.GetX()) + (a.GetZ() * a.GetZ())) * (b.GetZ() - c.GetZ()) + ((b.GetX() * b.GetX()) + (b.GetZ() * b.GetZ())) * (c.GetZ() - a.GetZ()) + ((c.GetX() * c.GetX()) + (c.GetZ() * c.GetZ())) * (a.GetZ() - b.GetZ()));
		z = (1 / D) * (((a.GetX() * a.GetX()) + (a.GetZ() * a.GetZ())) * (c.GetX() - b.GetX()) + ((b.GetX() * b.GetX()) + (b.GetZ() * b.GetZ())) * (a.GetX() - c.GetX()) + ((c.GetX() * c.GetX()) + (c.GetZ() * c.GetZ())) * (b.GetX() - a.GetX()));

		origo = Engine::Vertex(x, h, z);
	}

	void Triangle::Circle_Rad(Engine::Vertex a) {
		radious = sqrt(((((a.GetX() - origo.GetX()) * (a.GetX() - origo.GetX()))) + ((a.GetZ() - origo.GetZ()) * (a.GetZ() - origo.GetZ()))));
	}
//}