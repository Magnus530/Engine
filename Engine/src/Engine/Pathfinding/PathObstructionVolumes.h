#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Engine/Pathfinding/PNode.h"

namespace Engine {

	struct ObstructionSphereNodes
	{
		std::vector<int> m_nodes;
	};

	class PathObstructionSphereCollection
	{
	public:
		PathObstructionSphereCollection()
		{
			m_spherecollection = new std::vector<ObstructionSphereNodes>();
			m_SphereLocations = new std::vector<glm::vec3>();
			m_SphereRadiuses = new std::vector<float>();
		}
		~PathObstructionSphereCollection()
		{
			delete m_spherecollection;
			delete m_SphereLocations;
			delete m_SphereRadiuses;
		}

		uint32_t CreateObstructionSphere(float radius, const glm::vec3 location) 
		{
			m_spherecollection->emplace_back(ObstructionSphereNodes());
			m_SphereLocations->emplace_back(location);
			m_SphereRadiuses->emplace_back(radius);
			return m_spherecount++;
		}
		void EraseSphere(uint32_t index) 
		{
			m_spherecount--;
			auto& it = m_spherecollection->begin();
			(it += index);
			m_spherecollection->erase(it);

			auto& it2 = m_SphereLocations->begin();
			(it2 += index);
			m_SphereLocations->erase(it2);

			auto& it3 = m_SphereRadiuses->begin();
			(it3 += index);
			m_SphereRadiuses->erase(it3);
		}
		uint32_t								GetSphereCount()		{ return m_spherecount; }
		std::vector<ObstructionSphereNodes>&	GetSphereCollections()	{ return *m_spherecollection; }
		std::vector<glm::vec3>&					GetSphereLocations()	{ return *m_SphereLocations; }
		std::vector<float>&						GetSphereRadiuses()		{ return *m_SphereRadiuses; }
		std::vector<int>&						GetObstructionSphereNodes(uint32_t index)	{ return m_spherecollection->at(index).m_nodes; }
		void UpdateSphereData(uint32_t index, const float radius, const glm::vec3 location) 
		{ 
			m_SphereLocations->at(index) = location; 
			m_SphereRadiuses->at(index) = radius;
		}

	private:
		uint32_t m_spherecount{};
		std::vector<ObstructionSphereNodes>* m_spherecollection;
		std::vector<glm::vec3>* m_SphereLocations;
		std::vector<float>* m_SphereRadiuses;
	};

}