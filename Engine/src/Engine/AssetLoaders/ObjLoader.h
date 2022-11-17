#pragma once

//#include <Engine.h>

namespace Engine {

	class ObjLoader
	{
	public:
		//ObjLoader(std::string filename, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
		ObjLoader();
		~ObjLoader();

		static ObjLoader* Get();

		/* Vertexes are only positions for now */ // NEEDS AN OBJECT OR STRUCT LATER
		void ReadFile(std::string filename, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<uint32_t>& indices);
	};
}