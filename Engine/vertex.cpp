#include "epch.h"
#include "vertex.h"

std::ostream& operator << (std::ostream& os, const vertex& v) {
	os << std::fixed;
	os << "(" << v.xyz[0] << ", " << v.xyz[1] << ", " << v.xyz[2] << ")";
	os << "(" << v.normals[0] << ", " << v.normals[1] << ", " << v.normals[2] << ")";
	os << "(" << v.uv[0] << ", " << v.uv[1] << ")";
	return os;
}

std::istream& operator >> (std::istream& is, vertex& v) {
	char dum, dum1, dum2, dum3;
	is >> dum >> v.xyz[0] >> dum1 >> v.xyz[1] >> dum2 >> v.xyz[2] >> dum3;
	is >> dum >> v.normals[0] >> dum1 >> v.normals[1] >> dum2 >> v.normals[2] >> dum3;
	is >> dum >> v.uv[0] >> dum1 >> v.uv[1] >> dum2;
	return is;
}

vertex::vertex() {

}

vertex::vertex(float x, float y, float z, float r, float g, float b, float u, float v) {
	xyz[0] = x;
	xyz[1] = y;
	xyz[2] = z;
	normals[0] = r;
	normals[1] = g;
	normals[2] = b;
	uv[0] = u;
	uv[1] = v;
}

vertex::~vertex() {

}

float vertex::getX() {
	return xyz[0];
}

float vertex::getY() {
	return xyz[1];
}

float vertex::getZ() {
	return xyz[2];
}

void vertex::setColor(float r, float g, float b) {
	normals[0] = r;
	normals[1] = g;
	normals[2] = b;
}