/* The video by CppNuts on Youtube, was a helpful source when creating this class.
https://www.youtube.com/watch?v=XyNWEWUSa5E. This website is helpful in creating
a sphere http://www.songho.ca/opengl/gl_sphere.html */


#pragma once

#include "epch.h"
#include "glm/gtc/constants.hpp"
#include "Engine/Renderer/vertex.h"

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
			//    x      y	     z		 r	   g	 b			uv
				-0.5f, -0.5f,   0.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,	//	Bottom	- Left 
				 0.5f, -0.5f,   0.0f,	0.0f, 0.0f, 0.0f,	1.0f, 0.0f, //	Bottom	- Right
				 0.5f,  0.5f,   0.0f,	0.0f, 0.0f, 0.0f,	1.0f, 1.0f, //	Top		- Right
				-0.5f,  0.5f,   0.0f,	0.0f, 0.0f, 0.0f,	0.0f, 1.0f	//	Top		- Left
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
			// x      y	     z			// normals			  uv       
			-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, 0.0f,	  0.0f, 0.0f,	//	Bottom	- Left  // Front
			-0.5f,  0.5f, -0.5f,	0.0f,  0.0f, 0.0f,	  1.0f, 0.0f,   //	Bottom	- Right
			 0.5f,  0.5f, -0.5f,	0.0f,  0.0f, 0.0f,	  1.0f, 1.0f,   //	Top		- Right
			 0.5f, -0.5f, -0.5f,	0.0f,  0.0f, 0.0f,	  0.0f, 1.0f,	//	Top		- Left
														  
			-0.5f, -0.5f,  0.5f,	0.0f,  0.0f, 0.0f,	  0.0f, 0.0f,	//	Bottom	- Left //  Back
			-0.5f,  0.5f,  0.5f,	0.0f,  0.0f, 0.0f,	  1.0f, 0.0f,   //	Bottom	- Right
			 0.5f,  0.5f,  0.5f,	0.0f,  0.0f, 0.0f,	  1.0f, 1.0f,   //	Top		- Right
			 0.5f, -0.5f,  0.5f,	0.0f,  0.0f, 0.0f,	  0.0f, 1.0f	//	Top		- Left
		};

		std::vector<uint32_t> indices =
		{ 
			0, 1, 2,
			2, 3, 0,

			1, 5, 6,
			6, 2, 1,

			5, 4, 7,
			7, 6, 5,

			4, 0, 3,
			3, 7, 4,

			3, 2, 6,
			6, 7, 3,

			0, 1, 5,
			5, 4, 0
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
}