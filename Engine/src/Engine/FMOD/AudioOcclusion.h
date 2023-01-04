#pragma once
#include "glm/glm.hpp"
//#include "glad/glad.h"

namespace Engine {

	class AudioOcclusion
	{
	public:
		enum class Shape {
			Undefined = 0,
			Plane, 
			Box, 
			Icosahedron, 
			Custom
		};
		//Geometry ID assigned to this obstacle by the AudioEngine
		int geometryID;
		// Shape of the geometry 
		Shape shape;

		//flag to indicate whether or not to render this geometry
		bool render;

		// For now, the entire geometry has the same direct/reverb occlusion
		float directOcclusion;
		float reverbOcclusion;
		template<class T> using Array = std::vector<T>;
		virtual bool generatingBoundingBox(const Array<glm::vec3>& verts, glm::mat4 transformation) = 0;
		virtual bool generatingPlane(glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, glm::vec3 _v4, glm::mat4 transformation) = 0;

		Array<float> vertices;
		Array<uint32_t> indices;
		Array<glm::ivec3> faces;
	};
}
