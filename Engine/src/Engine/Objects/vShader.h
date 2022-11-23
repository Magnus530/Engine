//#pragma once
//
//#include <unordered_map>
//#include <string>
//#include <glm/glm.hpp>
//
//namespace Engine { 
//
//	class vShader
//	{
//	public:
//		vShader(const std::string& vertexpath, const std::string& fragmentpath, bool readfromfile = false);
//
//		void Use();
//
//		uint32_t GetProgram() const;
//
//		void SetUniformMatrix(const std::string& uniform, glm::mat4& matrix);
//		uint32_t GetUniformMatrix(const std::string& uniform);
//
//	private:
//		uint32_t m_Program;
//
//		std::unordered_map<std::string, uint32_t> m_Uniforms;
//	};
//}