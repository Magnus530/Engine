#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace Engine {

	//struct PathObstructionSphere
	//{
	//public:
	//	PathObstructionSphere(float r, glm::vec3 p)
	//		: m_radius{r}, m_position{p}{}

	//	void SetValues(float radius, glm::vec3 position) { 
	//		m_radius = radius; 
	//		m_position = position; 
	//	}

	//private:
	//	float m_radius{};
	//	glm::vec3 m_position{};

	//	void operator = (PathObstructionSphere p) { 
	//		m_radius = p.m_radius; 
	//		m_position = p.m_position; 
	//	}
	//};

	struct ObstructionSphereNodes
	{
		std::vector<std::shared_ptr<class PNode>> m_nodes;
	};

	class PathObstructionSphereCollection
	{
	public:
		uint32_t CreateObstructionSphere(float r, glm::vec3 p) {
			//m_spheres.emplace_back( r, p );
			m_spherecollection.emplace_back(ObstructionSphereNodes());
			return m_spherecount++;
		}
		//ObstructionSphereNodes& GetObstructionSpheres() { return m_spherecollection; }
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