#include "AresPCH.h"
#include "LayerStack.h"

namespace Ares {

	LayerStack::LayerStack()
	{
	}
	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_OverlayStartIndex, layer);
		m_OverlayStartIndex++;
	}

	void LayerStack::PushOverlay(Layer* layer)
	{
		m_Layers.emplace_back(layer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			m_OverlayStartIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
			m_Layers.erase(it);
	}
}
