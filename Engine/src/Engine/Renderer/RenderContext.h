#pragma once

#include "epch.h"
#include "RenderStrategy.h"

namespace Engine
{
	class RenderContext
	{
	private:
		std::unique_ptr<RenderStrategy> strategyPtr;

	public:
		explicit RenderContext(std::unique_ptr<RenderStrategy> && strategy = {}) : strategyPtr(std::move(strategy))
		{}

		void SetStrategy(std::unique_ptr<RenderStrategy>&& strategy)
		{
			strategy = std::move(strategy);
		}

		void ShaderInit(int shaderType) const
		{
			if (strategyPtr)
			{
				E_INFO("Context: sorting data using strategy.");
				strategyPtr->ShaderSelect();
			}
			else
			{
				E_INFO("Context: strategy has yet to be set.");
			}
		}
	};
}