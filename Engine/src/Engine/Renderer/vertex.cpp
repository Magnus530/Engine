#include "epch.h"
#include "vertex.h"

namespace Engine {

	std::ostream& operator << (std::ostream& os, const Vertex& v) {
		os << std::fixed;
		os << "(" << v.m_xyz[0] << ", " << v.m_xyz[1] << ", " << v.m_xyz[2] << ")";
		os << "(" << v.m_normals[0] << ", " << v.m_normals[1] << ", " << v.m_normals[2] << ")";
		os << "(" << v.m_uv[0] << ", " << v.m_uv[1] << ")";
		return os;
	}

	std::istream& operator >> (std::istream& is, Vertex& v) {
		char dum, dum1, dum2, dum3;
		is >> dum >> v.m_xyz[0] >> dum1 >> v.m_xyz[1] >> dum2 >> v.m_xyz[2] >> dum3;
		is >> dum >> v.m_normals[0] >> dum1 >> v.m_normals[1] >> dum2 >> v.m_normals[2] >> dum3;
		is >> dum >> v.m_uv[0] >> dum1 >> v.m_uv[1] >> dum2;
		return is;
	}

	Vertex::Vertex()
		: Vertex({ 0,0,0 }, { 0,0,0 }, { 0,0 })
	{
	}

	Vertex::Vertex(glm::vec3 position)
		: Vertex(position, { 0,0,0 }, { 0,0 })
	{
	}

	Vertex::Vertex(glm::vec3 position, glm::vec3 normal)
		: Vertex(position, normal, { 0,0 })
	{
	}

	Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv)
	{
		m_xyz[0] = position.x;
		m_xyz[1] = position.y;
		m_xyz[2] = position.z;
		m_normals[0] = normal.x;
		m_normals[1] = normal.y;
		m_normals[2] = normal.z;
		m_uv[0] = uv.x;
		m_uv[1] = uv.y;
	}

	Vertex::Vertex(float x, float y, float z, float r, float g, float b, float u, float v) {
		m_xyz[0] = x;
		m_xyz[1] = y;
		m_xyz[2] = z;
		m_normals[0] = r;
		m_normals[1] = g;
		m_normals[2] = b;
		m_uv[0] = u;
		m_uv[1] = v;
	}

	Vertex::~Vertex()
	{
	}

	glm::vec3 Vertex::GetPosition() const
	{
		return { m_xyz[0], m_xyz[1], m_xyz[2] };
	}

	glm::vec3 Vertex::GetNormal() const
	{
		return { m_normals[0], m_normals[1], m_normals[2] };
	}

	glm::vec2 Vertex::GetUv() const
	{
		return { m_uv[0], m_uv[1] };
	}

	void Vertex::SetPosition(float x, float y, float z) {
		m_xyz[0] = x;
		m_xyz[1] = y;
		m_xyz[2] = z;
	}

	void Vertex::SetNormal(float r, float g, float b) {
		m_normals[0] = r;
		m_normals[1] = g;
		m_normals[2] = b;
	}

	void Vertex::SetNormal(glm::vec3 normal)
	{
		m_normals[0] = normal.x;
		m_normals[1] = normal.y;
		m_normals[2] = normal.z;
	}
	void Vertex::SetUV(glm::vec2 uv)
	{
		m_uv[0] = uv.x;
		m_uv[1] = uv.y;
	}
}