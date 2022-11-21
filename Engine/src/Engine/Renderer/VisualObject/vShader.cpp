#include "epch.h"
//#include "vShader.h"
//
//#include <fstream>
//
//#include <glad/glad.h>
//#include <glm/gtc/type_ptr.hpp>
//
//namespace Engine {
//	vShader::vShader(const std::string& vertexpath, const std::string& fragmentpath, bool readfromfile/* = false*/)
//	{
//		std::string vertexCode;
//		std::string fragmentCode;
//		std::ifstream vShaderFile;
//		std::ifstream fShaderFile;
//
//		const GLchar* vShaderCode = vertexpath.c_str();
//		const GLchar* fShaderCode = fragmentpath.c_str();
//		if (readfromfile)
//		{
//			// Open files and check for errors
//			vShaderFile.open(vertexpath);
//			if (!vShaderFile)
//				E_ERROR("ERROR SHADER FILE {0} NOT SUCCESFULLY READ", vertexpath);
//			fShaderFile.open(fragmentpath);
//			if (!fShaderFile)
//				E_ERROR("ERROR SHADER FILE {0} NOT SUCCESFULLY READ", fragmentpath);
//			std::stringstream vShaderStream, fShaderStream;
//			// Read file's buffer contents into streams
//			vShaderStream << vShaderFile.rdbuf();
//			fShaderStream << fShaderFile.rdbuf();
//			// close file handlers
//			vShaderFile.close();
//			fShaderFile.close();
//			// Convert stream into string
//			vertexCode = vShaderStream.str();
//			fragmentCode = fShaderStream.str();
//
//			vShaderCode = vertexCode.c_str();
//			fShaderCode = fragmentCode.c_str();
//		}
//
//		// 2. Compile shaders
//		GLuint vertex, fragment;
//		GLint success;
//		GLchar infoLog[512];
//		// Vertex Shader
//		vertex = glCreateShader(GL_VERTEX_SHADER);
//		glShaderSource(vertex, 1, &vShaderCode, nullptr);
//		glCompileShader(vertex);
//		// Print compile errors if any
//		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
//		if (!success)
//		{
//			glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
//			E_ERROR("ERROR SHADER VERTEX {0} COMPILATION FAILED\n{1}", vertexpath, infoLog);
//		}
//		// Fragment Shader
//		fragment = glCreateShader(GL_FRAGMENT_SHADER);
//		glShaderSource(fragment, 1, &fShaderCode, nullptr);
//		glCompileShader(fragment);
//		// Print compile errors if any
//		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
//		if (!success)
//		{
//			glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
//			E_ERROR("ERROR SHADER VERTEX {0} COMPILATION FAILED\n{1}", fragmentpath, infoLog);
//		}
//
//		// Shader Program linking
//		this->m_Program = glCreateProgram();
//		glAttachShader(this->m_Program, vertex);
//		glAttachShader(this->m_Program, fragment);
//		glLinkProgram(this->m_Program);
//		// Print linking errors if any
//		glGetProgramiv(this->m_Program, GL_LINK_STATUS, &success);
//
//		//making nice output name for printing:
//		std::string shadername{ vertexpath };
//		shadername.resize(shadername.size() - 5); //deleting ".vert"
//
//		if (!success)
//		{
//			glGetProgramInfoLog(this->m_Program, 512, nullptr, infoLog);
//			E_ERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{0}\n{1}", shadername, infoLog);
//		}
//		else
//		{
//			E_INFO("GLSL shader {0} was successfully compiled");
//		}
//		// Delete the shaders as they're linked into our program now and no longer needed
//		// The shader program is now on the GPU and we reference it by using the mProgram variable
//		glDeleteShader(vertex);
//		glDeleteShader(fragment);
//	}
//	void vShader::Use()
//	{
//		glUseProgram(this->m_Program);
//	}
//	uint32_t vShader::GetProgram() const
//	{
//		return m_Program;
//	}
//	void vShader::SetUniformMatrix(const std::string& uniform, glm::mat4& matrix)
//	{
//		GLint location;
//		if (m_Uniforms.find(uniform) == m_Uniforms.end())
//		{
//			location = glGetUniformLocation(GetProgram(), uniform.c_str());
//			m_Uniforms[uniform] = location;
//		}
//		else
//		{
//			location = m_Uniforms[uniform];
//		}
//		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
//	}
//	uint32_t vShader::GetUniformMatrix(const std::string& uniform)
//	{
//		GLint location;
//		if (m_Uniforms.find(uniform) == m_Uniforms.end())
//		{
//			location = glGetUniformLocation(GetProgram(), uniform.c_str());
//			m_Uniforms[uniform] = location;
//		}
//		else
//		{
//			location = m_Uniforms[uniform];
//		}
//		return location;
//	}
//}
//
//
