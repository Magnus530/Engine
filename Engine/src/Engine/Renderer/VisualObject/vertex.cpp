#include "epch.h"
#include "vertex.h"

namespace Engine {

	std::ostream& operator << (std::ostream& os, const Vertex& v) {
		os << std::fixed;
		os << "(" << v.Xyz[0] << ", " << v.Xyz[1] << ", " << v.Xyz[2] << ")";
		os << "(" << v.Normals[0] << ", " << v.Normals[1] << ", " << v.Normals[2] << ")";
		os << "(" << v.Uv[0] << ", " << v.Uv[1] << ")";
		return os;
	}

	std::istream& operator >> (std::istream& is, Vertex& v) {
		char dum, dum1, dum2, dum3;
		is >> dum >> v.Xyz[0] >> dum1 >> v.Xyz[1] >> dum2 >> v.Xyz[2] >> dum3;
		is >> dum >> v.Normals[0] >> dum1 >> v.Normals[1] >> dum2 >> v.Normals[2] >> dum3;
		is >> dum >> v.Uv[0] >> dum1 >> v.Uv[1] >> dum2;
		return is;
	}

	Vertex::Vertex()
		: Vertex({ 0,0,0 }, { 1,1,1 }, { 1,1 })
	{
	}

	Vertex::Vertex(glm::vec3 position)
		: Vertex(position, { 1,1,1 }, { 1,1 })
	{
	}

	Vertex::Vertex(glm::vec3 position, glm::vec3 normal)
		: Vertex(position, normal, { 1,1 })
	{
	}

	Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv)
	{
		Xyz[0] = position.x;
		Xyz[1] = position.y;
		Xyz[2] = position.z;
		Normals[0] = normal.x;
		Normals[1] = normal.y;
		Normals[2] = normal.z;
		Uv[0] = uv.x;
		Uv[1] = uv.y;
	}

	Vertex::Vertex(float x, float y, float z, float r, float g, float b, float u, float v) {
		Xyz[0] = x;
		Xyz[1] = y;
		Xyz[2] = z;
		Normals[0] = r;
		Normals[1] = g;
		Normals[2] = b;
		Uv[0] = u;
		Uv[1] = v;
	}

	Vertex::~Vertex()
	{
	}

	glm::vec3 Vertex::GetPosition() const
	{
		return { Xyz[0], Xyz[1], Xyz[2] };
	}

	glm::vec3 Vertex::GetNormal() const
	{
		return { Normals[0], Normals[1], Normals[2] };
	}

	glm::vec2 Vertex::GetUv() const
	{
		return { Uv[0], Uv[1] };
	}

	float Vertex::GetX() const {
		return Xyz[0];
	}

	float Vertex::GetY() const {
		return Xyz[1];
	}

	float Vertex::GetZ() const {
		return Xyz[2];
	}

	float Vertex::GetR() const
	{
		return Normals[0];
	}

	float Vertex::GetG() const
	{
		return Normals[1];
	}

	float Vertex::GetB() const
	{
		return Normals[2];
	}

	void Vertex::SetNormal(float r, float g, float b) {
		Normals[0] = r;
		Normals[1] = g;
		Normals[2] = b;
	}

	void Vertex::SetNormal(glm::vec3 normal)
	{
		Normals[0] = normal.x;
		Normals[1] = normal.y;
		Normals[2] = normal.z;
	}
}