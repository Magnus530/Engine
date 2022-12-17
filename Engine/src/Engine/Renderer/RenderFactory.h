#pragma once

#include "epch.h"
#include "RenderType.h"

namespace Engine
{
	class RenderFactory
	{
	public:
		static std::shared_ptr<RenderType> CreateShaderType(const Engine::ShaderType& shaderType)
		{
			std::shared_ptr<RenderType> rType = nullptr;

			switch (shaderType)
			{
				case ShaderType::Flat:
				{
					rType = std::make_shared<Engine::FlatShader>();
					break;
				}
				case ShaderType::Texture:
				{
					rType = std::make_shared<Engine::TextureShader>();
					break;
				}
				case ShaderType::Phong:
				{
					rType = std::make_shared<Engine::PhongShader>();
					break;
				}
			}

			return rType;
		}
	};
}