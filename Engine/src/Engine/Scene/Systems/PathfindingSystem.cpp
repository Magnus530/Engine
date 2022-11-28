#include "epch.h"
#include "PathfindingSystem.h"

namespace Engine {

    //*************************************** PATHFINDING SYSTEM ********************************************//
	void PathfindingSystem::FindPath(PathfindingComponent& comp/*, std::shared_ptr<PNode> start, std::shared_ptr<PNode> end*/)
	{
        // Bit reduntant for pathfinding component to have array of node pointers, when it only uses their locations
        comp.m_CurrentPath = FindPath(comp.m_StartNode, comp.m_TargetNode, &comp.bBlocked, comp.m_IntermediateTargetNode);

        comp.m_SplinePath->m_Controlpoints.clear();
        comp.m_SplinePath->m_Controlpoints.push_back(comp.m_StartNode->m_Position);
        for (auto it = comp.m_CurrentPath.end(); it != comp.m_CurrentPath.begin();)
            comp.m_SplinePath->m_Controlpoints.push_back((*--it)->m_Position);
        
        // In case of only having two control points. Create a third in between, or else the fun
        if (comp.m_SplinePath->m_Controlpoints.size() == 2) {
            glm::vec3 start = comp.m_SplinePath->m_Controlpoints[0];
            glm::vec3 end = comp.m_SplinePath->m_Controlpoints[1];
            glm::vec3 middle = start + ((end - start) * 0.5f);
            comp.m_SplinePath->m_Controlpoints.insert(++comp.m_SplinePath->m_Controlpoints.begin(), middle);
        }


        BSplineCreator::MakeKnotVector(*comp.m_SplinePath);
        
        comp.m_SplineMovement = 0.f;    // Reset movement along spline
        comp.bReachedTarget = false;
	}

    void PathfindingSystem::MoveAlongPath(PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime)
    {
        float& t = pathfinder.m_SplineMovement;
        float speed = transform.m_Speed;

        if (t < 0.99f)
            t = std::clamp<float>(t += deltatime * speed, 0.f, 0.99f);
        else {
            t = 0.99f;

            if (pathfinder.bReachedTarget)
                pathfinder.m_StartNode = pathfinder.m_TargetNode;
            pathfinder.bReachedTarget = true;
        }
        
        if (!pathfinder.bReachedTarget) {
            glm::vec3 pos = BSplineCreator::GetPositionAlongSpline(*pathfinder.m_SplinePath, t);

            // tmp method
            // Transforming the Entity should NOT be done here. This function should instead just give a position along the path
            TransformSystem::SetWorldPosition(transform, pos + glm::vec3(0,1,0));   // Set position with extra height above the grid
        }
    }

    std::shared_ptr<PNode> PathfindingSystem::GetNodeClosestToPosition(uint32_t gridIndex, glm::vec3 position)
    {
        std::shared_ptr<NodeGrid> grid = NodeGridSystem::GetGridAtIndex(gridIndex);
    
        float length = glm::length(grid->m_Nodes[0]->m_Position - position);
        std::shared_ptr<PNode> node = grid->m_Nodes[0];
    
        for (size_t i{}; i < grid->m_Nodes.size(); i++)
        {
            float l = glm::length(grid->m_Nodes[i]->m_Position - position);
            if (l < length) {
                length = l;
                node = grid->m_Nodes[i];
            }
        }
        return node;
    }

	std::vector<std::shared_ptr<PNode>> PathfindingSystem::FindPath(std::shared_ptr<PNode> start, std::shared_ptr<PNode> end, bool* blocked, std::shared_ptr<PNode> intermediate)
    {
        std::vector<std::shared_ptr<PNode>> path;
        intermediate = nullptr;

        std::shared_ptr<PNode> closestNode{ start };     // If node is closed off, go to the closest one in, air length
        closestNode->InitValues(start.get(), end.get());
        std::vector<std::shared_ptr<PNode>> tosearch;
        tosearch.push_back(start);
        std::vector<std::shared_ptr<PNode>> processed;

        while (tosearch.size() > 0)
        {
            std::shared_ptr<PNode> current = tosearch[0];
            current->InitValues(start.get(), end.get());

            for (auto& it : tosearch)
            {
                if (it == current) { continue; }
                it->InitValues(start.get(), end.get());
                if (it->F < current->F || it->F == current->F && it->H < current->H)
                    current = it;
            }

            /* Setting closest node */
            if (closestNode->GetDistanceToNode(end.get()) > current->GetDistanceToNode(end.get()))
                closestNode = current;

            /* FOUND PATH: Setting Path */
            if (current.get() == end.get()) {
                /* Går bakover fra Target til Start */
                std::shared_ptr<PNode> currentTile = end;
                while (currentTile != start) {
                    path.push_back(currentTile);
                    currentTile = currentTile->m_Connection;
                }
                return path;
            }

            /* Removes current node from the to search list and adds it to processed */
            processed.push_back(current);
            std::vector<std::shared_ptr<PNode>>::iterator it = std::find(tosearch.begin(), tosearch.end(), current);
            if (it != tosearch.end()) {
                tosearch.erase(it);
            }

            /* Processes current node */
            for (size_t t{}; t < current->mConnectedNodes.size(); t++)
            {
                std::shared_ptr<PNode> neighbor = current->mConnectedNodes[t];
                auto it = std::find(tosearch.begin(), tosearch.end(), neighbor);
                bool inSearch = (it != tosearch.end());

                if (neighbor->IsBlock()) { continue; }

                it = std::find(processed.begin(), processed.end(), neighbor);
                if (it != processed.end()) { continue; }

                int CostToNeighbor = current->G + current->GetDistanceToNode(neighbor.get());

                if (!inSearch || CostToNeighbor < neighbor->G)
                {
                    neighbor->SetG(CostToNeighbor);
                    neighbor->m_Connection = current;

                    if (!inSearch) {
                        neighbor->SetH(end.get());
                        tosearch.push_back(neighbor);
                    }
                }
            }
        }
        /* If a path was not found, find the path to the closestNode */
        path = FindPath(start, closestNode);
        *blocked = true;
        intermediate = closestNode;
        return path;
    }


    void NodeGridSystem::CreateGrid()
    {
        CreateGridAtLocation(glm::vec3(0, 0, 0), 1.f, 10);
    }

    //********************************* NODE GRID SYSTEM ********************************//
    //NodeGridSystem::Grids* NodeGridSystem::m_Grids = new NodeGridSystem::Grids();
    static std::vector<std::shared_ptr<NodeGrid>> m_NodeGrids;

    std::shared_ptr<NodeGrid> NodeGridSystem::GetGridAtIndex(uint32_t index)
    {
        return m_NodeGrids[0];
    }
    std::shared_ptr<PNode> NodeGridSystem::GetNodeAtIndexWithinGrid(uint32_t gridIndex, uint32_t nodeIndex)
    {
        return m_NodeGrids[gridIndex]->m_Nodes[nodeIndex];
    }
    void NodeGridSystem::CreateGridAtLocation(glm::vec3 location, float gridSpacing, int gridSize)
    {
        std::vector<std::shared_ptr<PNode>> nodes;
        std::shared_ptr<PNode> a;
        std::shared_ptr<PNode> b;

        for (size_t x{}; x < 10; x++)
        {
            for (size_t z{}; z < 10; z++)
            {
                glm::vec3 loc = glm::vec3(location.x + (gridSpacing * x) - (float)gridSize / 2, 0, location.z + (gridSpacing * z) - (float)gridSize / 2);
                nodes.emplace_back(std::make_shared<PNode>(loc));
                //m_NodeLocations.push_back(location);
            }
        }
        for (size_t i{ 1 }; i < nodes.size(); i++)
        {
            if (nodes.size() < i + 1) { break; }

            a = nodes[i - 1];
            b = nodes[i];
            if (i % 10 != 0 || i == 0) {
                a->AddConnectedNode(b);
                b->AddConnectedNode(a);
            }

            if (nodes.size() < i + 10) { continue; }
            b = nodes[(i - 1) + 10];
            b->AddConnectedNode(a);
            a->AddConnectedNode(b);
        }
        m_NodeGrids.emplace_back(std::make_shared<NodeGrid>(location, glm::vec3(10, 0, 10), nodes));
        GenerateNodeNamesForGrid(m_NodeGrids[m_NodeGrids.size() - 1]);
    }

    void NodeGridSystem::GenerateNodeNamesForGrid(std::shared_ptr<NodeGrid> grid)
    {
        std::string name = "PNode ";
        uint32_t i{};
        for (auto& it : grid->m_Nodes) {
            i++;
            it->m_name = name + std::to_string(i);
        }
    }

}
