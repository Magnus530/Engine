/* The video by CppNuts on Youtube, was a helpful source when creating this class.
https://www.youtube.com/watch?v=XyNWEWUSa5E. This website is helpful in creating
a sphere http://www.songho.ca/opengl/gl_sphere.html */


#pragma once

#include <vector>
#include "glm/gtc/constants.hpp"

namespace Engine
{
	class PrimitiveShape
	{
	public:
		virtual ~PrimitiveShape() = 0;

		virtual std::vector<float> GetVertices() { return vertices; }
		virtual std::vector<uint32_t> GetIndices() { return indices; }

	private:
		std::vector<float> vertices;
		std::vector<uint32_t> indices;
	};

	PrimitiveShape::~PrimitiveShape() {}

	class Plane : public PrimitiveShape
	{
	public:
		~Plane() {};

		virtual std::vector<float> GetVertices() override { return vertices; }
		virtual std::vector<uint32_t> GetIndices() override { return indices; }

	private:
		std::vector<float> vertices =
		{
			//    x      y	     z			uv
				-0.5f, -0.5f,   0.0f,	0.0f, 0.0f,	//	Bottom	- Left 
				 0.5f, -0.5f,   0.0f,	1.0f, 0.0f, //	Bottom	- Right
				 0.5f,  0.5f,   0.0f,	1.0f, 1.0f, //	Top		- Right
				-0.5f,  0.5f,   0.0f,	0.0f, 1.0f	//	Top		- Left
		};

		std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };
	};

	class Cube : public PrimitiveShape
	{
	public:
		~Cube() {};

		virtual std::vector<float> GetVertices() override { return vertices; }
		virtual std::vector<uint32_t> GetIndices() override { return indices; }

	private:
		std::vector<float> vertices =
		{
			//    x      y	     z			uv       
				-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,	//	Bottom	- Left  // Front
				-0.5f,  0.5f, -0.5f,	1.0f, 0.0f, //	Bottom	- Right
				 0.5f,  0.5f, -0.5f,	1.0f, 1.0f, //	Top		- Right
				 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//	Top		- Left
													
				-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,	//	Bottom	- Left //  Back
				-0.5f,  0.5f,  0.5f,	1.0f, 0.0f, //	Bottom	- Right
				 0.5f,  0.5f,  0.5f,	1.0f, 1.0f, //	Top		- Right
				 0.5f, -0.5f,  0.5f,	0.0f, 1.0f	//	Top		- Left
		};

		std::vector<uint32_t> indices =
		{ 
			0, 1, 2, 2, 3, 0,
			1, 5, 6, 6, 2, 1,
			5, 4, 7, 7, 6, 5,
			4, 0, 3, 3, 7, 4,

			3, 2, 6, 6, 7, 3,
			0, 1, 5, 5, 4, 0
		};
	};

	class Icosahedron : public PrimitiveShape
	{
	public:
		~Icosahedron() {};

		virtual std::vector<float> GetVertices() override { return vertices; }
		virtual std::vector<uint32_t> GetIndices() override { return indices; }

	private:
		const float X = 0.525731112119133606f;
		const float Z = .850650808352039932f;
		const float N = 0.f;

		std::vector<float> vertices =
		{
				-X, N,  Z,	0.0f, 0.0f,
				 X, N,  Z,	1.0f, 0.0f,
				-X, N, -Z,	1.0f, 1.0f,
				 X, N, -Z,	0.0f, 1.0f,

				 N,  Z,  X,	0.0f, 0.0f,
				 N,  Z, -X,	1.0f, 0.0f,
				 N, -Z,  X,	1.0f, 1.0f,
				 N, -Z, -X,	0.0f, 1.0f,

				 Z,  X,  N,	0.0f, 0.0f,
				-Z,  X,  N,	1.0f, 0.0f,
				 Z, -X,  N,	1.0f, 1.0f,
				-Z, -X,  N,	0.0f, 1.0f
		};

		std::vector<uint32_t> indices =
		{
			0, 4, 1,
			0, 9, 4,
			9, 5, 4,
			4, 5, 8,
			4, 8, 1,

			8, 10, 1,
			8, 3, 10,
			5, 3, 8,
			5, 2, 3,
			2, 7, 3,

			7, 10, 3,
			7, 6, 10,
			7, 11, 6,
			11, 0, 6,
			0, 1, 6,

			6, 1, 10,
			9, 0, 11,
			9, 11, 2,
			9, 2, 5,
			7, 2, 11
		};
	};

	class Sphere : public PrimitiveShape
	{
	public:
		~Sphere() {};

		virtual std::vector<float> GetVertices() override { GenVertices(1.0f, 36, 18); return vertices; }
		virtual std::vector<uint32_t> GetIndices() override { GenIndices(36, 18); return indices; }

		void GenVertices(int radius, int sectorCount, int stackCount)
		{
			float x, y, z, xy;                              // vertex position
			float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
			float s, t;                                     // vertex texCoord

			float sectorStep = 2 * PI / sectorCount;
			float stackStep = PI / stackCount;
			float sectorAngle, stackAngle;

			for (int i = 0; i <= stackCount; ++i)
			{
				stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
				xy = radius * cosf(stackAngle);             // r * cos(u)
				z = radius * sinf(stackAngle);              // r * sin(u)

				// add (sectorCount+1) vertices per stack
				// the first and last vertices have same position and normal, but different tex coords
				for (int j = 0; j <= sectorCount; ++j)
				{
					sectorAngle = j * sectorStep;           // starting from 0 to 2pi

					// vertex position (x, y, z)
					x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
					y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
					vertices.push_back(x);
					vertices.push_back(y);
					vertices.push_back(z);

					// normalized vertex normal (nx, ny, nz)
					nx = x * lengthInv;
					ny = y * lengthInv;
					nz = z * lengthInv;
					vertices.push_back(nx);
					vertices.push_back(ny);
					vertices.push_back(nz);

					// vertex tex coord (s, t) range between [0, 1]
					s = (float)j / sectorCount;
					t = (float)i / stackCount;
					vertices.push_back(s);
					vertices.push_back(t);
				}
			}
		}

		void GenIndices(int sectorCount, int stackCount)
		{
			// generate CCW index list of sphere triangles
			// k1--k1+1
			// |  / |
			// | /  |
			// k2--k2+1
			//std::vector<int> indices;
			//std::vector<int> lineIndices;
			int k1, k2;
			for (int i = 0; i < stackCount; ++i)
			{
				k1 = i * (sectorCount + 1);     // beginning of current stack
				k2 = k1 + sectorCount + 1;      // beginning of next stack

				for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
				{
					// 2 triangles per sector excluding first and last stacks
					// k1 => k2 => k1+1
					if (i != 0)
					{
						indices.push_back(k1);
						indices.push_back(k2);
						indices.push_back(k1 + 1);
					}

					// k1+1 => k2 => k2+1
					if (i != (stackCount - 1))
					{
						indices.push_back(k1 + 1);
						indices.push_back(k2);
						indices.push_back(k2 + 1);
					}

					// store indices for lines
					// vertical lines for all stacks, k1 => k2
					//lineIndices.push_back(k1);
					//lineIndices.push_back(k2);
					indices.push_back(k1);
					indices.push_back(k2);

					if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
					{
						//lineIndices.push_back(k1);
						//lineIndices.push_back(k1 + 1);
						indices.push_back(k1);
						indices.push_back(k1 + 1);
					}
				}
			}
		}

	private:
		const float PI = 3.14159265358979323846264338327950288;

		std::vector<float> vertices;
		std::vector<uint32_t> indices;
	};
}