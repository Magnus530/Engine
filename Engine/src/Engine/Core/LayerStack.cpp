#include "epch.h"
#include "LayerStack.h"

/* Layers are pushed into the first half of the list, overlays are pushed into the
second half of the list. */

namespace Engine
{
	LayerStack::LayerStack()
	{}

	/* Layers exist even when they are popped, and are only destoyed by the destructor. */ 

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
		}
	}
	std::vector<std::string> LayerStack::GetLayerNames() const
	{
		std::vector<std::string> names;
		for (const auto& it : m_Layers)
		{
			names.push_back(it->GetName());
		}
		return names;
	}
	Layer* LayerStack::GetLayerAtIndex(uint32_t index)
	{
		if (index > m_Layers.size() + 1)
			return nullptr;
		return m_Layers[index];
	}
}