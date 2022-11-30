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
			m_nodecollection.emplace_back(ObstructionSphereNodes());
			return m_spherecount++;
			//return m_spheres.size() - 1;	// returns ObstructionSphere ID
		}
		//PathObstructionSphere GetObstructionSphere(uint32_t index) const { return m_spheres[index]; }
		std::vector<std::shared_ptr<class PNode>>& GetObstructionSphereNodes(uint32_t index) { return m_nodecollection[index].m_nodes; }


	private:
		uint32_t m_spherecount{};
		//std::vector<PathObstructionSphere> m_spheres;
		std::vector<ObstructionSphereNodes> m_nodecollection;
	};

}