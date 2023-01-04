#pragma once
#include "Engine/FMOD/AudioOcclusion.h"
//#include <Engine/AssetInit/PrimitiveShape.h>


namespace Engine {
	
	class AudioGeometry : public AudioOcclusion {
	public:
		AudioGeometry();

		//bool generatingBoundingBox(const Array<glm::vec3>& verts, glm::mat4 transformation = glm::mat4(1.f)) override;
		//bool generatingPlane(glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, glm::vec3 _v4, glm::mat4 transformation) override;
		//
		//inline const std::vector<float>&      GetVertices() override{ return vertices; }
		//inline const std::vector<uint32_t>&   GetIndices()  override{ return indices; }
		//inline const std::vector<glm::ivec3>& getFaces() { return faces; }
	private:
		bool generateIndicesFromTirangles(const Array<glm::ivec3>& _faces);
	};
}
