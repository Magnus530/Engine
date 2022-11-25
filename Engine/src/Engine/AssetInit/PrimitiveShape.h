#pragma once

#include <vector>

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
}