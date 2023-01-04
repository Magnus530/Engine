#include <glm/glm.hpp>
#include <string>

#include "epch.h"
#include "Terrain.h"

#include <iostream>
#include <fstream>
#include <cmath>

//namespace Engine
//{
	int Terrain::to_int(std::string s) {
		int k{ 0 };
		int b{ 0 };
		for (int i = 0; i < s.size(); i++) {
			k *= 10;
			b = int(s[i]) - 48;
			k += b;
			if (s[i] = ' ') {
				return k;
			}
		}
	}

	float Terrain::to_float(std::string s) {
		float k{ 0 };
		int a{ 0 };
		int b{ 0 };
		bool counta{ false };
		for (int i = 0; i < s.size(); i++) {
			if (counta) {
				a++;
			}
			if (s[i] == '(') {

			}
			else if (s[i] == '.') {
				counta = true;
			}
			else {
				k *= 10;
				b = int(s[i]) - 48;
				k += b;
			}
		}
		for (int i = 0; i < a; i++) {
			k = k / 10;
		}
		return k;
	}

	Terrain::Terrain() {
		float x{ 0 }, y{ 0 }, z{ 0 };
		std::string filepath = "assets/meshes/Test_shape.txt";

		std::string word = "";
		std::string line = "";
		int n{ 0 };

		std::ifstream fileinn;

		fileinn.open(filepath.c_str());
		/*if (fileinn.is_open()) {
			char tea;
			int n;
			fileinn >> n;
			for (int i = 0; i < n; i++) {
				/*fileinn >> tea;
				fileinn >> x;
				fileinn >> tea;
				fileinn >> y;
				fileinn >> tea;
				fileinn >> z;*/

		/*Engine::Vertex v /*= Engine::Vertex(x, y, z)*/;
		/*fileinn >> v;
		mVertices.push_back(v);
	}
	fileinn.close();
}*/

//manual to test
		mVertices.push_back(Engine::Vertex(13.6, 5.8, 1.27));
		mVertices.push_back(Engine::Vertex(3.41, 19.72, 10));
		mVertices.push_back(Engine::Vertex(2.82, 2.24, 3.05));
		mVertices.push_back(Engine::Vertex(7.4, 14.17, 4));
		mVertices.push_back(Engine::Vertex(26.37, 3.5, 1.1));
		mVertices.push_back(Engine::Vertex(25.99, 18.73, 2.7));
		mVertices.push_back(Engine::Vertex(37.73, 8.99, 2.58));
		mVertices.push_back(Engine::Vertex(30.44, 26.18, 6));
		mVertices.push_back(Engine::Vertex(13.94, 25.04, 3.52));
		mVertices.push_back(Engine::Vertex(38, 22, 3.46));

		for (int i = 0; i < mVertices.size(); i++) {
			x = mVertices[i].GetX();
			y = mVertices[i].GetY();
			z = mVertices[i].GetZ();

			mVertices[i].SetPosition(x, z, y);
		}


		float xMax = mVertices[0].GetX(), xMin = mVertices[0].GetX(), yMax = mVertices[0].GetY(), yMin = mVertices[0].GetY(), zMax = mVertices[0].GetZ(), zMin = mVertices[0].GetZ();

		for (int i = 0; i < mVertices.size(); i++) {
			if (mVertices[i].GetX() < xMin) {
				xMin = mVertices[i].GetX();
			}
			if (mVertices[i].GetX() > xMax) {
				xMax = mVertices[i].GetX();
			}
			if (mVertices[i].GetY() < yMin) {
				yMin = mVertices[i].GetY();
			}
			if (mVertices[i].GetY() > yMax) {
				yMax = mVertices[i].GetY();
			}
			if (mVertices[i].GetZ() < zMin) {
				zMin = mVertices[i].GetZ();
			}
			if (mVertices[i].GetZ() < zMin) {
				zMax = mVertices[i].GetZ();
			}
		}

		int size{ 0 };
		for (int i = 0; i < mVertices.size(); i++) {
			x = mVertices[i].GetX();
			y = mVertices[i].GetY();
			z = mVertices[i].GetZ();
			mVertices[i].SetPosition(x - xMin, y - yMin, z - zMin);
			size++;
		}
		xMax -= xMin;
		xMin -= xMin;
		yMax -= yMin;
		yMin -= yMin;
		zMax -= zMin;
		zMin -= zMin;

		int p_id[3] = {0, 0, 0};
		const float k = 10;
		mVertices.push_back(Engine::Vertex(xMin - k, 0, zMin));
		p_id[0] = size;
		size++;

		mVertices.push_back(Engine::Vertex(xMax + k, 0, zMin));
		p_id[1] = size;
		size++;

		z = xMax + zMin;
		x = (xMax - xMin) / 2;
		float a1 = x / k;
		z = a1 * z;

		mVertices.push_back(Engine::Vertex(x, 0, z + zMax));
		p_id[2] = size;
		size++;
		int siz = mVertices.size();

		CreateTriangles(p_id[0], p_id[1], p_id[2]);

		/**/int c{0};
		int s = Triangles.size() - 1;
		while (c != s) {
			if ((Triangles[c].point[0] == p_id[0] || Triangles[c].point[0] == p_id[1] || Triangles[c].point[0] == p_id[2]) || (Triangles[c].point[1] == p_id[0] || Triangles[c].point[1] == p_id[1] || Triangles[c].point[1] == p_id[2]) || (Triangles[c].point[2] == p_id[0] || Triangles[c].point[2] == p_id[1] || Triangles[c].point[2] == p_id[2])) {
				Triangle temporary = Triangles[c];
				Triangles[c] = Triangles[s];
				Triangles[s] = temporary;
				Triangles.pop_back();
				s--;
			}
			else {
				c++;
			}
		}/**/

		for (int i = 0; i < Triangles.size(); i++) {
			mIndices.push_back(Triangles[i].point[0]);
			mIndices.push_back(Triangles[i].point[1]);
			mIndices.push_back(Triangles[i].point[2]);
		}

		for (int i = 0; i < mVertices.size(); i++) {
			float colour = mVertices[i].GetY() / yMax;
			mVertices[i].SetNormal(colour, colour, colour);
		}
	}

	void Terrain::init(std::vector<Engine::Vertex>& vertices, std::vector<uint32_t>& indices) {
		vertices = mVertices;
		indices = mIndices;
	}

	void Terrain::CreateTriangles(int i1, int i2, int i3) {
		Triangle tem_tri = Triangle(i1, i2, i3, mVertices);
		Triangles.push_back(tem_tri);
		int c{ 0 };
		int g{ 1 };
		while (c != g) {
			if (Triangles[c].havechild()) {
				g = Creation(c, Triangles.size()); //slightly wrong
				//g = BayericCreation(c, Triangles.size()); //slightly wrong in a diffrent way
			}
			c++;
		}
		/**/int l{0};
		int s = Triangles.size() - 1;
		while (l != s) {
			if (Triangles[l].havechild()) {
				Triangle temp = Triangles[l];
				Triangles[l] = Triangles[s];
				Triangles[s] = temp;
				Triangles.pop_back();
				s--;
			}
			else {
				l++;
			}
		}/**/
	}

	int Terrain::BayericCreation(int c, int g) {
		for (int a = 0; a < Triangles[c].amountofchildren(); a++) {
			bool e{ false };
			glm::vec2 va = { mVertices[Triangles[c].point[0]].GetX(), mVertices[Triangles[c].point[0]].GetZ() };
			glm::vec2 vb = { mVertices[Triangles[c].point[1]].GetX(), mVertices[Triangles[c].point[1]].GetZ() };
			glm::vec2 vc = { mVertices[Triangles[c].point[2]].GetX(), mVertices[Triangles[c].point[2]].GetZ() };
			glm::vec3 bay = bayercentricCoordinates(va, vb, vc, { mVertices[Triangles[c].currentchild(a)].GetX(), mVertices[Triangles[c].currentchild(a)].GetZ() });
			if (bay.z == 0) { //ligg på ab
				Triangle te = Triangle(Triangles[c].point[2], Triangles[c].point[0], Triangles[c].currentchild(a), mVertices);
				for (int d = 0; d < g; d++) {
					if (Triangles[d].center().GetPosition() == te.center().GetPosition() && Triangles[d].rad() == te.rad()) {
						e = true;
					}
				}
				if (!e) {
					Triangles.push_back(te);
					g++;
				}
				e = false;
				Triangle tem = Triangle(Triangles[c].point[1], Triangles[c].point[2], Triangles[c].currentchild(a), mVertices);
				for (int d = 0; d < g; d++) {
					if (Triangles[d].center().GetPosition() == tem.center().GetPosition() && Triangles[d].rad() == tem.rad()) {
						e = true;
					}
				}
				if (!e) {
					Triangles.push_back(tem);
					g++;
				}
				e = false;
			}
			else if (bay.x == 0) { //ligg på bc
				Triangle te = Triangle(Triangles[c].point[0], Triangles[c].point[1], Triangles[c].currentchild(a), mVertices);
				for (int d = 0; d < g; d++) {
					if (Triangles[d].center().GetPosition() == te.center().GetPosition() && Triangles[d].rad() == te.rad()) {
						e = true;
					}
				}
				if (!e) {
					Triangles.push_back(te);
					g++;
					e = false;
				}
				Triangle tem = Triangle(Triangles[c].point[2], Triangles[c].point[0], Triangles[c].currentchild(a), mVertices);
				for (int d = 0; d < g; d++) {
					if (Triangles[d].center().GetPosition() == tem.center().GetPosition() && Triangles[d].rad() == tem.rad()) {
						e = true;
					}
				}
				if (!e) {
					Triangles.push_back(tem);
					g++;
					e = false;
				}
			}
			else if (bay.y == 0) { //ligg på ca
				Triangle te = Triangle(Triangles[c].point[0], Triangles[c].point[1], Triangles[c].currentchild(a), mVertices);
				for (int d = 0; d < g; d++) {
					if (Triangles[d].center().GetPosition() == te.center().GetPosition() && Triangles[d].rad() == te.rad()) {
						e = true;
					}
				}
				if (!e) {
					Triangles.push_back(te);
					g++;
					e = false;
				}
				Triangle tem = Triangle(Triangles[c].point[1], Triangles[c].point[2], Triangles[c].currentchild(a), mVertices);
				for (int d = 0; d < g; d++) {
					if (Triangles[d].center().GetPosition() == tem.center().GetPosition() && Triangles[d].rad() == tem.rad()) {
						e = true;
					}
				}
				if (!e) {
					Triangles.push_back(tem);
					g++;
					e = false;
				}
			}
			else {
				Triangle te = Triangle(Triangles[c].point[0], Triangles[c].point[1], Triangles[c].currentchild(a), mVertices);
				for (int d = 0; d < g; d++) {
					if ((Triangles[d].center().GetPosition() == te.center().GetPosition()) && (Triangles[d].rad() == te.rad())) {
						e = true;
					}
				}
				if (!e) {
					Triangles.push_back(te);
					g++;
					e = false;
				}
				Triangle tem = Triangle(Triangles[c].point[1], Triangles[c].point[2], Triangles[c].currentchild(a), mVertices);
				for (int d = 0; d < g; d++) {
					if (Triangles[d].center().GetPosition() == tem.center().GetPosition() && Triangles[d].rad() == tem.rad()) {
						e = true;
					}
				}
				if (!e) {
					Triangles.push_back(tem);
					g++;
					e = false;
				}
				Triangle t = Triangle(Triangles[c].point[2], Triangles[c].point[0], Triangles[c].currentchild(a), mVertices);
				for (int d = 0; d < g; d++) {
					if (Triangles[d].center().GetPosition() == t.center().GetPosition() && Triangles[d].rad() == t.rad()) {
						e = true;
					}
				}
				if (!e) {
					Triangles.push_back(t);
					g++;
					e = false;
				}
			}
		}
		return g;
	}

	int Terrain::Creation(int c, int g) {
		for (int a = 0; a < Triangles[c].amountofchildren(); a++) {
			bool e{ false };

			glm::vec2 A, B, C, D, AC, AB, BC, AD, BD;

			A = { mVertices[Triangles[c].point[0]].GetX(), mVertices[Triangles[c].point[0]].GetZ() };
			B = { mVertices[Triangles[c].point[1]].GetX(), mVertices[Triangles[c].point[1]].GetZ() };
			C = { mVertices[Triangles[c].point[2]].GetX(), mVertices[Triangles[c].point[2]].GetZ() };

			AB = A - B;
			AC = A - C;
			BC = B - C;

			float ma{ 0 }, mb{ 0 }, mc{ 0 };
			if (AB.y != 0) {
				if (AB.x == 0) {
					ma = 0;
				}
				else {
					ma = AB.x / AB.y;
				}
			}
			if (BC.y != 0) {
				if (BC.x == 0) {
					mb = 0;
				}
				else {
					mb = BC.x / BC.y;
				}
			}
			if (AC.y != 0) {
				if (AC.x == 0) {
					mc = 0;
				}
				else {
					mc = AC.x / AC.y;
				}
			}

			float x = mVertices[Triangles[c].currentchild(a)].GetX();
			float y = mVertices[Triangles[c].currentchild(a)].GetZ();
			D = { x, y };
			AD = A - D;
			BD = B - D;

			float o1, o2;

			if (AD.y != 0 && BD.y != 0) {
				if (AD.x == 0) {
					o1 = 0;
				}
				else {
					o1 = AD.x / AD.y;
				}
				if (BD.x == 0) {
					o2 = 0;
				}
				else {
					o2 = BD.x / BD.y;
				}

				if (o1 == ma) { //ligg på ab
					Triangle temptri = Triangle(Triangles[c].point[2], Triangles[c].point[0], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
					e = false;
					temptri = Triangle(Triangles[c].point[1], Triangles[c].point[2], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
				}
				else if (o2 == mb) { //ligg på bc
					Triangle temptri = Triangle(Triangles[c].point[0], Triangles[c].point[1], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
					e = false;
					temptri = Triangle(Triangles[c].point[2], Triangles[c].point[0], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
				}
				else if (o1 == mc) {
					Triangle temptri = Triangle(Triangles[c].point[0], Triangles[c].point[1], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
					e = false;
					temptri = Triangle(Triangles[c].point[1], Triangles[c].point[2], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
				}
				else {
					Triangle temptri = Triangle(Triangles[c].point[0], Triangles[c].point[1], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
					e = false;
					temptri = Triangle(Triangles[c].point[1], Triangles[c].point[2], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
					e = false;
					temptri = Triangle(Triangles[c].point[2], Triangles[c].point[0], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
				}
			}
			else {
				if (AB.y == 0 && A.y == D.y) {
					Triangle temptri = Triangle(Triangles[c].point[2], Triangles[c].point[0], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
					e = false;
					temptri = Triangle(Triangles[c].point[1], Triangles[c].point[2], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
				}
				if (AC.y == 0 && A.y == D.y) {
					Triangle temptri = Triangle(Triangles[c].point[0], Triangles[c].point[1], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
					e = false;
					temptri = Triangle(Triangles[c].point[1], Triangles[c].point[2], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
				}
				if (BC.y == 0 && B.y == D.y) {
					Triangle temptri = Triangle(Triangles[c].point[0], Triangles[c].point[1], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
					e = false;
					temptri = Triangle(Triangles[c].point[2], Triangles[c].point[0], Triangles[c].currentchild(a), mVertices);
					for (int d = 0; d < Triangles.size(); d++) {
						if (Triangles[d].center().GetPosition() == temptri.center().GetPosition() && Triangles[d].rad() == temptri.rad()) {
							e = true;
						}
					}
					if (!e) {
						Triangles.push_back(temptri);
						g++;
					}
				}
			}
		}
		return g;
	}

	glm::vec3 Terrain::bayercentricCoordinates(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d) {
		glm::vec3 ba = { b.x - a.x, 0, b.y - a.y };
		glm::vec3 ca = { c.x - a.x, 0, c.y - a.y };
		glm::vec3 n = glm::cross(ba, ca);
		if (n.y < 0) {
			n.y = n.y * (-1);
		}
		float areal = n.y;

		glm::vec3 bayc;

		//u
		glm::vec3 bd = { b.x - d.x, 0, b.y - d.y };
		glm::vec3 cd = { c.x - d.x, 0, c.y - d.y };
		n = glm::cross(bd, cd);
		if (n.y < 0) {
			n.y = n.y * (-1);
		}
		if (n.y != 0) {
			bayc.x = n.y / areal;
		}
		else {
			bayc.x = 0;
		}

		//v
		//glm::vec3 cd = { c.x - d.x, 0, c.y - d.y };
		glm::vec3 ad = { a.x - d.x, 0, a.y - d.y };
		n = glm::cross(cd, ad);
		if (n.y < 0) {
			n.y = n.y * (-1);
		}
		if (n.y != 0) {
			bayc.y = n.y / areal;
		}
		else {
			bayc.y = 0;
		}

		//w
		//glm::vec3 ad = { a.x - d.x, 0, a.y - d.y };
		//glm::vec3 bd = { b.x - d.x, 0, b.y - d.y };
		n = glm::cross(ad, bd);
		if (n.y < 0) {
			n.y = n.y * (-1);
		}
		if (n.y != 0) {
			bayc.z = n.y / areal;
		}
		else {
			bayc.z = 0;
		}

		return bayc;
	}
//}