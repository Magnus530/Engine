#pragma once
#include <vector>

#include "../Renderer/vertex.h"

//namespace Engine {

	class Triangle
	{
	public:
		Triangle() {};
		Triangle(int index_1, int index_2, int index_3, std::vector <Engine::Vertex> vertices);
		~Triangle() {};

		Engine::Vertex center() const { return origo; };
		float rad() const { return radious; };
		bool havechild() const { return child; };
		int currentchild(int index) const { return PotentialChildren[index]; };
		int amountofchildren() { return PotentialChildren.size(); };

		int point[3] = { 0, 0, 0 };
	private:
		Engine::Vertex origo;
		float radious;
		bool child;
		std::vector <int> PotentialChildren;

		void FindCenter(Engine::Vertex a, Engine::Vertex b, Engine::Vertex c);
		void Circle_Rad(Engine::Vertex a);
	};
//}

