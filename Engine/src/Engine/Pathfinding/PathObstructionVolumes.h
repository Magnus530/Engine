#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace Engine {

	struct ObstructionSphereNodes
	{
		std::vector<std::shared_ptr<class PNode>> m_nodes;
	};

	class PathObstructionSphereCollection
	{
	public:
		uint32_t CreateObstructionSphere(float r, glm::vec3 p) {
			m_spherecollection.emplace_back(ObstructionSphereNodes());
			return m_spherecount++;
		}
		void EraseSphere(uint32_t index) {
			m_spherecount--;
			auto& it = m_spherecollection.begin();
			(it += index);
			m_spherecollection.erase(it);
		}
		std::vector<ObstructionSphereNodes>& GetSphereCollections() { return m_spherecollection; }
		std::vector<std::shared_ptr<class PNode>>& GetObstructionSphereNodes(uint32_t index) { return m_spherecollection[index].m_nodes; }


	private:
		uint32_t m_spherecount{};
		std::vector<ObstructionSphereNodes> m_spherecollection;
	};

}