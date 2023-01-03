#include "epch.h"
#include "AudioOcclusion.h"

#include <iterator>

namespace Engine {
	AudioGeometry::AudioGeometry()
	{
		geometryID = -1;
	}

	bool AudioGeometry::generatedBoundingBox(const Array<glm::vec3>& verts, glm::mat4 transformation)
	{
		 // If there are no vertices then it cannot generate a bounding box
		if (verts.size() == 0) return false;

		//find the min/max xyz respectively
		glm::vec3 t = verts[0]; glm::vec3 u = verts[0];
		for (const glm::vec3 v : verts) 
		{
			t.x = std::min(t.x, v.x);
			t.y = std::min(t.y, v.y);
			t.z = std::min(t.z, v.z);
			u.x = std::max(u.x, v.x);
			u.y = std::max(u.y, v.y);
			u.z = std::max(u.z, v.z);
		}

		// Given min/max vertices, generate the 6 other corners and generate faces / indices for lines as follows:
		//      6_______ 5
		//      /|     /| 
		//     / |    / |
		//   7/__|__4/__|1
		//    |  /2  |  /
		//    | /    | /
		//    |/_____|/
		//   3       0

		vertices = {
			{u.x, t.y, t.z}, 
			{u.x, t.y, u.z}, 
			{t.x, t.y, u.z},
			t,
			{u.x, u.y, t.z}, 
			u,
			{t.x, u.y, u.z}, 
			{t.x, u.y, t.z}

		};
		for (auto& vertex : vertices) {
			vertex = transformation * glm::vec4((vertex, 1.f));
		}
		faces = {
			{1,2,3},
			{7,6,5},
			{4,5,1},
			{5,6,2},
			{2,6,7},
			{0,3,7},
			{0,1,3},
			{4,7,5},
			{0,4,1},
			{1,5,2},
			{3,2,7},
			{4,0,7}
		};

	}
	bool AudioGeometry::generatedPlane(glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, glm::vec3 _v4, glm::mat4 transformation)
	{
		vertices = { _v1, _v2, _v3, _v4 };
		faces = {
			{0,1,2},
			{0,2,3}
		};
		return false;
	}
	bool AudioGeometry::generateIndicesFromTirangles(const Array<glm::vec3>& _faces)
	{
		uint32_t nExistingIndices = (uint32_t)indices.size();
		indices.resize(indices.size() + _faces.size() * 6);

		for (uint32_t f = 0; f < _faces.size(); f++) {
			glm::vec3 face = _faces[f];
			uint32_t start = nExistingIndices + f * 6;

			indices[start + 0] = indices[start + 5] = face[0];
			indices[start + 1] = indices[start + 2] = face[1];
			indices[start + 3] = indices[start + 4] = face[2];

		}

		return true;
	}
}