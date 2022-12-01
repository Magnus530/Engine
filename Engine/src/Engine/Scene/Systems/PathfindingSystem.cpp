#include "epch.h"
#include "PathfindingSystem.h"

namespace Engine {

    //************************************************************************ PATHFINDING SYSTEM **********************************************************************************//
	void PathfindingSystem::FindPath(PathfindingComponent& comp, const glm::vec3 currentPosition)
	{
        //** Init Pathfinding
        const bool NoMovement = comp.m_StartNode == comp.m_TargetNode;
        if (NoMovement) return;
        
        if (comp.bStartedPathfinding)
            comp.m_StartNode = PathfindingSystem::GetNodeClosestToPosition(0, currentPosition);
        comp.bStartedPathfinding = true;    // In init or FindPath?
        //*** END Init Pathfinding ***

        // Bit reduntant for pathfinding component to have array of node pointers, when it only uses their locations
        comp.m_CurrentPath = FindPath(comp.m_StartNode, comp.m_TargetNode, comp.m_IntermediateTargetNode, &comp.bIsObstructed);
        const bool NoMovIntermediate = comp.m_StartNode == comp.m_IntermediateTargetNode;
        if (NoMovIntermediate) {
            comp.bStartedPathfinding = false;
            comp.bReachedTarget = true;
            return;
        }

        comp.m_SplinePath->m_Controlpoints.clear();
        comp.m_SplinePath->m_Controlpoints.push_back(currentPosition);
        for (auto it = comp.m_CurrentPath.end(); it != comp.m_CurrentPath.begin();)
            comp.m_SplinePath->m_Controlpoints.push_back((*--it)->m_Data->m_Position);
        
        // In case of only having two control points. Create a third in between, or else the spline functions doesn't work.
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
            // Transforming the Entity should NOT be done here. This function should instead just give a intended position along the path
            TransformSystem::SetWorldPosition(transform, pos + glm::vec3(0,0.5f,0));   // Manually adding extra height
        }
    }

    // INEFFICIENT - Needs a proper indexed nodegrid to improve search time
    std::shared_ptr<PNode> PathfindingSystem::GetNodeClosestToPosition(uint32_t gridIndex, glm::vec3 position)
    {
        std::shared_ptr<NodeGrid> grid = NodeGridSystem::GetGridAtIndex(gridIndex);
    
        float length = glm::length(grid->m_Nodes[0]->m_Data->m_Position - position);
        std::shared_ptr<PNode> node = grid->m_Nodes[0];
    
        for (size_t i{}; i < grid->m_Nodes.size(); i++)
        {
            float l = glm::length(grid->m_Nodes[i]->m_Data->m_Position - position);
            if (l < length) {
                length = l;
                node = grid->m_Nodes[i];
            }
        }
        return node;
    }

	std::vector<std::shared_ptr<PNode>> PathfindingSystem::FindPath(std::shared_ptr<PNode> start, std::shared_ptr<PNode> end, std::shared_ptr<PNode>& intermediate, bool* blocked)
    {
        std::vector<std::shared_ptr<PNode>> path;
        intermediate = nullptr;
        
        if (start == end)
            return path;


        std::shared_ptr<PNode> closestNode{ start };     // If node is closed off, go to the closest one in, air length
        closestNode->InitValues(end.get());
        std::vector<std::shared_ptr<PNode>> tosearch;
        tosearch.push_back(start);
        std::vector<std::shared_ptr<PNode>> processed;

        while (tosearch.size() > 0)
        {
            std::shared_ptr<PNode> current = tosearch[0];
            current->InitValues(end.get());

            /* Choosing node to process from the insearch array */
            for (auto& it : tosearch)
            {
                if (it == current) { continue; }
                it->InitValues(/*start.get(), */end.get());
                if (it->m_DistanceValues->F < current->m_DistanceValues->F || it->m_DistanceValues->F == current->m_DistanceValues->F && it->m_DistanceValues->H < current->m_DistanceValues->H)
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

            /* Removes the chosen node from tosearch and adds it to processed */
            processed.push_back(current);
            std::vector<std::shared_ptr<PNode>>::iterator it = std::find(tosearch.begin(), tosearch.end(), current);
            if (it != tosearch.end()) {
                tosearch.erase(it);
            }

            /* Processes chosen node */
            for (size_t t{}; t < current->m_Neighbors->neighbors.size(); t++)
            {
                std::shared_ptr<PNode> neighbor = current->m_Neighbors->neighbors[t];
                auto it = std::find(tosearch.begin(), tosearch.end(), neighbor);
                bool inSearch = (it != tosearch.end());

                if (neighbor->IsObstruction()) { continue; }

                it = std::find(processed.begin(), processed.end(), neighbor);
                if (it != processed.end()) { continue; }

                int CostToNeighbor = current->m_DistanceValues->G + current->GetDistanceToNode(neighbor.get());

                if (!inSearch || CostToNeighbor < neighbor->m_DistanceValues->G)
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
        path = FindPath(start, closestNode, intermediate);
        *blocked = true;
        intermediate = closestNode;
        return path;
    }


    void NodeGridSystem::CreateGrid()
    {
        CreateGridAtLocation(glm::vec3(0, 0, 0), 1.f, 10);
    }

    //************************************************************************* NODE GRID SYSTEM ***************************************************************************************************//
    //NodeGridSystem::Grids* NodeGridSystem::m_Grids = new NodeGridSystem::Grids();
    static std::vector<std::shared_ptr<NodeGrid>> m_NodeGrids;

    /* Nodes, that potentially, are no longer obstructions */
    static std::vector<std::shared_ptr<PNode>> m_PotentiallyFalseObstructions;

    std::shared_ptr<NodeGrid> NodeGridSystem::GetGridAtIndex(uint32_t index)
    {
        return m_NodeGrids[0];
    }
    std::shared_ptr<PNode> NodeGridSystem::GetNodeAtIndexWithinGrid(uint32_t gridIndex, uint32_t nodeIndex)
    {
        return m_NodeGrids[gridIndex]->m_Nodes[nodeIndex];
    }

    uint32_t NodeGridSystem::CreateObstructionSphere(uint32_t gridIndex, float radius, glm::vec3 position)
    {
        NodeGrid* grid = m_NodeGrids[gridIndex].get();
        uint32_t sphereIndex = grid->m_ObstructionSpheres.CreateObstructionSphere(radius, position);
        // Get indexed grid
        std::vector<std::shared_ptr<PNode>>& nodes = grid->m_Nodes;

        // Set nodes within the sphere to Obstructions
        UpdateObstructionSphere(gridIndex, sphereIndex, radius, position);
        return sphereIndex;
    }

    void NodeGridSystem::DeleteObstructionSphere(uint32_t gridIndex, uint32_t sphereIndex)
    {
        NodeGrid* grid = m_NodeGrids[gridIndex].get();
        std::vector<std::shared_ptr<PNode>> nodes = grid->m_ObstructionSpheres.GetObstructionSphereNodes(sphereIndex);
        for (const auto it : nodes)
            m_PotentiallyFalseObstructions.push_back(it);
        grid->m_ObstructionSpheres.EraseSphere(sphereIndex);
    }   

    void NodeGridSystem::UpdateObstructionSphere(uint32_t gridIndex, uint32_t sphereIndex, float radius, glm::vec3 position)
    {
        int floatToInt = 1000;
        int intRadius = (int)(radius * floatToInt);

        // Remove nodes outside of new sphere radius
        NodeGrid* grid = m_NodeGrids[gridIndex].get();
        std::vector<std::shared_ptr<PNode>>& nodes = grid->m_ObstructionSpheres.GetObstructionSphereNodes(sphereIndex);
        for (const auto& it : nodes)
        {
            //float distance = glm::length(position - it->m_Data->m_Position);
            int distance = (int)(glm::length(position - it->m_Data->m_Position) * floatToInt);
            if (distance > intRadius) {
                if (std::find(m_PotentiallyFalseObstructions.begin(), m_PotentiallyFalseObstructions.end(), it) == m_PotentiallyFalseObstructions.end())
                    m_PotentiallyFalseObstructions.push_back(it);
            }
        }
        nodes.clear();

        // Set nodes within the sphere to Obstructions
        std::vector<std::shared_ptr<PNode>> gridnodes = grid->m_Nodes;
        for (auto& it : gridnodes)
        {
            //float distance = glm::length(position - it->m_Data->m_Position);
            //float distance = glm::length(position - it->m_Data->m_Position);
            int distance = (int)(glm::length(position - it->m_Data->m_Position) * floatToInt);
            if (distance <= intRadius) {
                it->SetObstructionStatus(true);
                nodes.push_back(it);
            }
        }
        //UpdateFalseObstructionNodes(gridIndex);
    }

    void NodeGridSystem::UpdateFalseObstructionNodes(uint32_t gridIndex)
    {
        if (m_PotentiallyFalseObstructions.size() == 0) return;

        auto& nodeGrids = m_NodeGrids[gridIndex];
        std::vector<std::shared_ptr<PNode>> nodes;

        for (const auto& it : nodeGrids->m_ObstructionSpheres.GetSphereCollections())
            for (const auto& it2 : it.m_nodes)
                    nodes.push_back(it2);

        for (auto it = m_PotentiallyFalseObstructions.begin(); it != m_PotentiallyFalseObstructions.end();)
        {
            bool b{};
            for (const auto& it2 : nodes) {
                if (it2 == (*it)) {
                    m_PotentiallyFalseObstructions.erase(it);
                    b = true;
                    break;
                }
            }
            if (m_PotentiallyFalseObstructions.size() == 0) break;
            if (!b) (++it);
        }
            
        for (auto& obs : m_PotentiallyFalseObstructions)
            obs->SetObstructionStatus(false);
        m_PotentiallyFalseObstructions.clear();
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
        std::string baseName = "PNode ";
        uint32_t i{};
        for (auto& it : grid->m_Nodes) {
            i++;
            std::string name = baseName + std::to_string(i);
            it->m_Data->m_Name = name;
        }
    }

}
