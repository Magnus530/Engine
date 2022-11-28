#pragma once

#include "PrimitiveShape.h"
#include <memory>

namespace Engine
{
	class PrimitiveShapeFactory
	{
	public:
		static std::shared_ptr<PrimitiveShape> CreatePrimitiveShape(int shape)
		{
			std::shared_ptr<PrimitiveShape> pShape = nullptr;

			switch (shape)
			{
				case 1:
				{
					pShape = std::make_shared<Engine::Plane>();
					break;
				}
				case 2:
				{
					pShape = std::make_shared<Engine::Cube>();
					break;
				}
				case 3:
				{
					pShape = std::make_shared<Engine::Icosahedron>();
					break;
				}
				case 4:
				{
					pShape = std::make_shared<Engine::Sphere>(2, 1.0f);
					break;
				}
				default:
				{
					E_CORE_INFO("Please enter another number for PrimitiveShape type.");
					return nullptr;
				}
			}

			return pShape;
		}
	};
}