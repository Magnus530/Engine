#include "epch.h"
#include "PathfindingSystem.h"

namespace Engine {
    static std::vector<std::shared_ptr<NodeGrid>> m_NodeGrids;

    //************************************************************************ PATHFINDING SYSTEM **********************************************************************************//
	void PathfindingSystem::FindPath(PathfindingComponent& comp, const glm::vec3 currentPosition)
	{
        //** Init Pathfinding
        const bool NoMovement = comp.m_StartNode == comp.m_TargetNode;
        if (NoMovement) return;

        auto& grid = m_NodeGrids[0];
        
        if (comp.bIsMovingAlongPath)
            comp.m_StartNode = PathfindingSystem::GetNodeClosestToPosition(0, currentPosition);
        comp.bIsMovingAlongPath = true;    // In init or FindPath?
        //*** END Init Pathfinding ***

        // Bit reduntant for pathfinding component to have array of node pointers, when it only uses their locations
        comp.m_CurrentPath = FindPathAStar(0, comp.m_StartNode, comp.m_TargetNode, comp.m_IntermediateTargetNode, &comp.bIsObstructed);
        const bool NoMovIntermediate = comp.m_StartNode == comp.m_IntermediateTargetNode;
        if (NoMovIntermediate) {
            comp.bIsMovingAlongPath = false;
            comp.bReachedTarget = true;
            return;
        }

        // Adding spline control points to pathfinding component
        comp.m_SplinePath->m_Controlpoints.clear();
        comp.m_SplinePath->m_Controlpoints.push_back(currentPosition);
        for (auto it = comp.m_CurrentPath.end(); it != comp.m_CurrentPath.begin();)
            comp.m_SplinePath->m_Controlpoints.push_back(grid->m_NodeLocations->at(*--it));
            //comp.m_SplinePath->m_Controlpoints.push_back((*--it)->m_Data->m_Position);

        
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


        // Get length of spline path for accurate movement speed through spline
        comp.m_SplineLength = 0.f;
        glm::vec3 prevPos = currentPosition;
        for (float t{}; t < 1.f; t += 0.05)
        {
            glm::vec3 pos = BSplineCreator::GetPositionAlongSpline(*comp.m_SplinePath, t);

            float l = glm::length(pos - prevPos);
            comp.m_SplineLength += l;

            prevPos = BSplineCreator::GetPositionAlongSpline(*comp.m_SplinePath, t);
        }
	}

    void PathfindingSystem::PathMovement(PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime)
    {
    }

    void PathfindingSystem::MoveAlongPath(PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime)
    {
        if (!pathfinder.bIsMovingAlongPath) return;
        if (pathfinder.bReachedTarget) return;

        //float prevT = pathfinder.m_SplineMovement;
        float& t = pathfinder.m_SplineMovement;
        float speed = pathfinder.m_EntityPathSpeed / pathfinder.m_SplineLength;
        if (speed <= 0.f) return;

        if (t < 0.99f) {
            t = std::clamp<float>(t += deltatime * speed, 0.f, 0.99f);
            //pathfinder.m_CurrentPatrolPoint += t - prevT;
        }
        else {  // Reached end of path
            //t = 0.99f;
            t = 0.f;
            pathfinder.m_StartNode = pathfinder.m_TargetNode;
            pathfinder.bReachedTarget = true;

            if (pathfinder.bPatrolling) {
                PatrolUpdate(pathfinder, transform.GetPosition());
                return;
            }
        }
        
        if (!pathfinder.bReachedTarget) 
        {
            glm::vec3 previousPosition = transform.GetPosition();
            glm::vec3 pos = BSplineCreator::GetPositionAlongSpline(*pathfinder.m_SplinePath, t);

            glm::vec3 direction = pos - previousPosition;
            direction.y = 0.f;

            // tmp method
            // Transforming the Entity should NOT be done here. This function should instead just give a intended position along the path
            TransformSystem::SetWorldPosition(transform, pos + glm::vec3(0,0.5f,0));   // Manually adding extra height
            TransformSystem::RotateToDirectionVector(transform, glm::normalize(direction));
        }
    }

    void PathfindingSystem::ResumeMovementAlongPath(PathfindingComponent& pathfinder, const glm::vec3 currentPosition)
    {
        pathfinder.bIsMovingAlongPath = true;
        pathfinder.m_StartNode = GetNodeClosestToPosition(0, currentPosition);
        FindPath(pathfinder, currentPosition);
    }

    void PathfindingSystem::CancelMovementAlongPath(PathfindingComponent& pathfinder)
    {
        pathfinder.bIsMovingAlongPath = false;
        pathfinder.m_CurrentPath.clear();
    }

    void PathfindingSystem::MoveAlongPatrol(PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime)
    {
    }

    void PathfindingSystem::StartPatrol(PathfindingComponent& pathfinder, const glm::vec3 currentPosition, PatrolType patroltype/* = PatrolType::Loop*/)
    {
        if (pathfinder.m_PatrolPath.size() < 2)
            return;

        // Called at start of patrol and each time entity reaches their target
            // Sets next point in patrolpath
        pathfinder.bPatrolling = true;
        pathfinder.m_PatrolType = patroltype;
        pathfinder.m_StartNode = PathfindingSystem::GetNodeClosestToPosition(0, currentPosition);  // Kan også finne ut av hvilken punkt den er nærmest, og starte der. For å kunne restarte patrol
        PatrolUpdate(pathfinder, currentPosition);
    }

    void PathfindingSystem::PatrolUpdate(PathfindingComponent& pathfinder, const glm::vec3 currentPosition)
    {
        switch (pathfinder.m_PatrolType)
        {
        case Engine::Single: 
        {
            pathfinder.m_CurrentPatrolPoint++;
            if (pathfinder.m_CurrentPatrolPoint >= pathfinder.m_PatrolPath.size())
                return;
            break;
        }
        case Engine::Loop:
        {
            pathfinder.bReachedTarget = false;
            pathfinder.m_CurrentPatrolPoint++;
            if (pathfinder.m_CurrentPatrolPoint >= pathfinder.m_PatrolPath.size())
                pathfinder.m_CurrentPatrolPoint = 0;
            break;
        }
        case Engine::Reverse:
        {
            pathfinder.bReachedTarget = false;
            pathfinder.bReverse ? pathfinder.m_CurrentPatrolPoint-- : pathfinder.m_CurrentPatrolPoint++;
            if (pathfinder.m_CurrentPatrolPoint == pathfinder.m_PatrolPath.size())
                pathfinder.m_CurrentPatrolPoint --;
            else if (pathfinder.m_CurrentPatrolPoint < 0)
                pathfinder.m_CurrentPatrolPoint = 0;
            break;
        }
        default:
            break;
        }
        pathfinder.m_TargetNode = PathfindingSystem::GetNodeClosestToPosition(0, pathfinder.m_PatrolPath[pathfinder.m_CurrentPatrolPoint]);
        FindPath(pathfinder, currentPosition - glm::vec3(0, 0.5f, 0));
    }

    void PathfindingSystem::PausePatrol(PathfindingComponent& pathfinder, const glm::vec3 currentPosition)
    {
        pathfinder.bPatrolling = false;
        pathfinder.m_PatrolPauseLocation = currentPosition;
        CancelMovementAlongPath(pathfinder);
    }

    void PathfindingSystem::ResumePatrol(PathfindingComponent& pathfinder, const glm::vec3 currentPosition, PatrolType patroltype)
    {
        pathfinder.bPatrolling = true;
        pathfinder.bIsMovingAlongPath = true;
        pathfinder.m_PatrolType = patroltype;
        pathfinder.m_StartNode = GetNodeClosestToPosition(0, currentPosition);
        pathfinder.m_TargetNode = PathfindingSystem::GetNodeClosestToPosition(0, pathfinder.m_PatrolPath[pathfinder.m_CurrentPatrolPoint]);
        FindPath(pathfinder, currentPosition - glm::vec3(0, 0.5f, 0));
    }

    void PathfindingSystem::AddPatrolPath(PathfindingComponent& pathfinder, std::vector<glm::vec3> patrolpath)
    {
        for (const auto& it : patrolpath)
            pathfinder.m_PatrolPath.push_back(it);
    }

    void PathfindingSystem::AddToPatrolPath(PathfindingComponent& pathfinder, glm::vec3 pos)
    {
        pathfinder.m_PatrolPath.push_back(pos);
    }

    void PathfindingSystem::AddToPatrolPathAt(PathfindingComponent& pathfinder, glm::vec3 pos, unsigned int index)
    {
        if (pathfinder.m_PatrolPath.size() < index) return;

        unsigned int i{};
        for (auto& it = pathfinder.m_PatrolPath.begin(); it != pathfinder.m_PatrolPath.end();)
        {
            if (i == index) {
                pathfinder.m_PatrolPath.insert(it, pos);
                return;
            }
            i++;
        }
    }

    void PathfindingSystem::ClearPatrol(PathfindingComponent& pathfinder)
    {
        pathfinder.bPatrolling = false;
        pathfinder.m_CurrentPatrolPoint = 0;
        pathfinder.m_PatrolPath.clear();
    }

    // INEFFICIENT - Needs a properly indexed nodegrid to improve search time
    //std::shared_ptr<PNode> PathfindingSystem::GetNodeClosestToPosition(uint32_t gridIndex, glm::vec3 position)
    int PathfindingSystem::GetNodeClosestToPosition(uint32_t gridIndex, glm::vec3 position)
    {
        std::shared_ptr<NodeGrid> grid = NodeGridSystem::GetGridAtIndex(gridIndex);
    
        //float length = glm::length(grid->m_Nodes[0]->m_Data->m_Position - position);
        float length = glm::length(grid->m_NodeLocations->at(0) - position);
        int nodeIndex{ 0 };
        //std::shared_ptr<PNode> node = grid->m_Nodes[0];
    
        for (size_t i{}; i < grid->m_NodeLocations->size(); i++)
        {
            //float l = glm::length(grid->m_Nodes[i]->m_Data->m_Position - position);
            float l = glm::length(grid->m_NodeLocations->at(i) - position);
            if (l < length) {
                length = l;
                //node = grid->m_Nodes[i];
                nodeIndex = i;
            }
        }
        return nodeIndex;
    }

    std::vector<int> PathfindingSystem::FindPathAStar(int gridIndex, int startNode, int endNode, int& intermediateNode, bool* blocked)
    {
        std::vector<int> path;
        auto& grid = m_NodeGrids[gridIndex];
        intermediateNode = -1;

        if (startNode == endNode)
            return path;


        //std::shared_ptr<PNode> closestNode{ start };     // If node is closed off, go to the closest one in, air length
        int closestNode{ startNode };     // If node is closed off, go to the closest one in, air length
        grid->NodeInitValues(closestNode, endNode);
        //closestNode->InitValues(end.get());
        std::vector<int> tosearch;
        tosearch.push_back(startNode);
        std::vector<int> processed;

        while (tosearch.size() > 0)
        {
            int current = tosearch[0];
            grid->NodeInitValues(current, endNode);
            //current->InitValues(end.get());

            /* Choosing node to process from the insearch array */
            for (auto& it : tosearch)
            {
                if (it == current) { continue; }
                grid->NodeInitValues(it, endNode);
                //it->InitValues(/*start.get(), */end.get());
                //if (it->m_DistanceValues->F < current->m_DistanceValues->F || it->m_DistanceValues->F == current->m_DistanceValues->F && it->m_DistanceValues->H < current->m_DistanceValues->H)
                auto& d = grid->m_NodeDN->m_DistanceValues;
                if (d[it].F < d[current].F || d[it].F == d[current].F && d[it].H < d[current].H)
                    current = it;
            }

            /* Setting closest node */
            //if (closestNode->GetDistanceToNode(end.get()) > current->GetDistanceToNode(end.get()))
            if (grid->GetNodeDistanceToNode(closestNode, endNode) > grid->GetNodeDistanceToNode(current, endNode))
                closestNode = current;

            /* FOUND PATH: Setting Path */
            if (current == endNode) {
                /* Går bakover fra Target til Start */
                int currentTile = endNode;
                while (currentTile != startNode) {
                    path.push_back(currentTile);
                    //currentTile = currentTile->m_Connection;
                    currentTile = grid->m_NodePathConnection->at(currentTile);
                }
                return path;
            }

            /* Removes the chosen node from tosearch and adds it to processed */
            processed.push_back(current);
            //std::vector<std::shared_ptr<PNode>>::iterator it = std::find(tosearch.begin(), tosearch.end(), current);
            auto it = std::find(tosearch.begin(), tosearch.end(), current);
            if (it != tosearch.end()) {
                tosearch.erase(it);
            }

            /* Processes chosen node */
            //for (size_t t{}; t < current->m_Neighbors->neighbors.size(); t++)
            //for (auto& i : grid->m_NodeDN->m_NeighborIndices)
            size_t t{ 0 };
            for (t = 0; t < grid->m_NodeDN->m_NeighborIndices[current].size() - 1; t++)
            {
                //if (*it == -1) continue;

                //std::shared_ptr<PNode> neighbor = current->m_Neighbors->neighbors[t];
                int neighbor = grid->m_NodeDN->m_NeighborIndices[current][t];
                auto& it = std::find(tosearch.begin(), tosearch.end(), neighbor);
                bool inSearch = (it != tosearch.end());

                //if (neighbor->IsObstruction()) { continue; }
                if (grid->m_NodeObstructionStatus->at(neighbor)) { continue; }

                it = std::find(processed.begin(), processed.end(), neighbor);
                if (it != processed.end()) { continue; }

                //int CostToNeighbor = current->m_DistanceValues->G + current->GetDistanceToNode(neighbor.get());
                int CostToNeighbor = grid->m_NodeDN->m_DistanceValues[current].G + grid->GetNodeDistanceToNode(current, endNode);

                //if (!inSearch || CostToNeighbor < neighbor->m_DistanceValues->G)
                if (!inSearch || CostToNeighbor < grid->m_NodeDN->m_DistanceValues[neighbor].G)
                {
                    //neighbor->SetG(CostToNeighbor);
                    //neighbor->m_Connection = current;
                    grid->m_NodeDN->m_DistanceValues[neighbor].G = CostToNeighbor;
                    grid->m_NodePathConnection->at(neighbor) = current;

                    if (!inSearch) {
                        //neighbor->SetH(end.get());
                        grid->m_NodeDN->m_DistanceValues[neighbor].H = grid->GetNodeDistanceToNode(neighbor, endNode);
                        tosearch.push_back(neighbor);
                    }
                }
            }
        }
        /* If a path was not found, find the path to the closestNode */
        //bool b{};
        path = FindPathAStar(gridIndex, startNode, closestNode, intermediateNode/*, &b*/);
        *blocked = true;
        intermediateNode = closestNode;
        return path;
    }

	//std::vector<std::shared_ptr<PNode>> PathfindingSystem::FindPathAStar(std::shared_ptr<PNode> start, std::shared_ptr<PNode> end, std::shared_ptr<PNode>& intermediate, bool* blocked)
    //std::vector<int> FindPathAStar(int gridIndex, int startNode, int endNode, int& intermediateNode, bool* blocked)
    //{
    //    std::vector<int> path;
    //    auto& grid = m_NodeGrids[gridIndex];
    //    intermediateNode = -1;
    //    
    //    if (startNode == endNode)
    //        return path;
    //
    //
    //    //std::shared_ptr<PNode> closestNode{ start };     // If node is closed off, go to the closest one in, air length
    //    int closestNode{ startNode };     // If node is closed off, go to the closest one in, air length
    //    grid->NodeInitValues(closestNode, endNode);
    //    //closestNode->InitValues(end.get());
    //    std::vector<int> tosearch;
    //    tosearch.push_back(startNode);
    //    std::vector<int> processed;
    //
    //    while (tosearch.size() > 0)
    //    {
    //        int current = tosearch[0];
    //        grid->NodeInitValues(current, endNode);
    //        //current->InitValues(end.get());
    //
    //        /* Choosing node to process from the insearch array */
    //        for (auto& it : tosearch)
    //        {
    //            if (it == current) { continue; }
    //            grid->NodeInitValues(it, endNode);
    //            //it->InitValues(/*start.get(), */end.get());
    //            //if (it->m_DistanceValues->F < current->m_DistanceValues->F || it->m_DistanceValues->F == current->m_DistanceValues->F && it->m_DistanceValues->H < current->m_DistanceValues->H)
    //            auto& d = grid->m_NodeDN->m_DistanceValues;
    //            if (d[it].F < d[current].F || d[it].F == d[current].F && d[it].H < d[current].H)
    //                current = it;
    //        }
    //
    //        /* Setting closest node */
    //        //if (closestNode->GetDistanceToNode(end.get()) > current->GetDistanceToNode(end.get()))
    //        if (grid->GetNodeDistanceToNode(closestNode, endNode) > grid->GetNodeDistanceToNode(current, endNode))
    //            closestNode = current;
    //
    //        /* FOUND PATH: Setting Path */
    //        if (current == endNode) {
    //            /* Går bakover fra Target til Start */
    //            int currentTile = endNode;
    //            while (currentTile != startNode) {
    //                path.push_back(currentTile);
    //                //currentTile = currentTile->m_Connection;
    //                currentTile = grid->m_NodePathConnection->at(currentTile);
    //            }
    //            return path;
    //        }
    //
    //        /* Removes the chosen node from tosearch and adds it to processed */
    //        processed.push_back(current);
    //        //std::vector<std::shared_ptr<PNode>>::iterator it = std::find(tosearch.begin(), tosearch.end(), current);
    //        auto it = std::find(tosearch.begin(), tosearch.end(), current);
    //        if (it != tosearch.end()) {
    //            tosearch.erase(it);
    //        }
    //
    //        /* Processes chosen node */
    //        //for (size_t t{}; t < current->m_Neighbors->neighbors.size(); t++)
    //        //for (auto& i : grid->m_NodeDN->m_NeighborIndices)
    //        for (size_t t{}; t < grid->m_NodeDN->m_NeighborIndices.size(); t++)
    //        {
    //            //if (*it == -1) continue;
    //
    //            //std::shared_ptr<PNode> neighbor = current->m_Neighbors->neighbors[t];
    //            int neighbor = grid->m_NodeDN->m_NeighborIndices[current][t];
    //            auto& it = std::find(tosearch.begin(), tosearch.end(), neighbor);
    //            bool inSearch = (it != tosearch.end());
    //
    //            //if (neighbor->IsObstruction()) { continue; }
    //            if (grid->m_NodeObstructionStatus->at(*it)) { continue; }
    //
    //            it = std::find(processed.begin(), processed.end(), neighbor);
    //            if (it != processed.end()) { continue; }
    //
    //            //int CostToNeighbor = current->m_DistanceValues->G + current->GetDistanceToNode(neighbor.get());
    //            int CostToNeighbor = grid->m_NodeDN->m_DistanceValues[current].G + grid->GetNodeDistanceToNode(current, endNode);
    //
    //            //if (!inSearch || CostToNeighbor < neighbor->m_DistanceValues->G)
    //            if (!inSearch || CostToNeighbor < grid->m_NodeDN->m_DistanceValues[neighbor].G)
    //            {
    //                //neighbor->SetG(CostToNeighbor);
    //                //neighbor->m_Connection = current;
    //                grid->m_NodeDN->m_DistanceValues[neighbor].G = CostToNeighbor;
    //                grid->m_NodePathConnection->at(neighbor) = current;
    //
    //                if (!inSearch) {
    //                    //neighbor->SetH(end.get());
    //                    grid->m_NodeDN->m_DistanceValues[neighbor].H = grid->GetNodeDistanceToNode(neighbor, endNode);
    //                    tosearch.push_back(neighbor);
    //                }
    //            }
    //        }
    //    }
    //    /* If a path was not found, find the path to the closestNode */
    //    bool b{};
    //    path = FindPathAStar(gridIndex, startNode, closestNode, intermediateNode, &b);
    //    *blocked = true;
    //    intermediateNode = closestNode;
    //    return path;
    //}



    //************************************************************************* NODE GRID SYSTEM ***************************************************************************************************//

    /* Nodes, that potentially, are no longer obstructions */
    //static std::vector<int> m_PotentiallyFalseObstructions;  // TODO: make separate object. Created on Updating ObstructionComponent 
    static std::vector<ObstructionUpdates> m_ObstructionUpdates;

    std::shared_ptr<NodeGrid> NodeGridSystem::GetGridAtIndex(uint32_t index)
    {
        return m_NodeGrids[0];
    }
    glm::vec3 NodeGridSystem::GetNodeLocation(int gridIndex, int NodeIndex)
    {
        return m_NodeGrids[gridIndex]->m_NodeLocations->at(NodeIndex);
    }

    uint32_t NodeGridSystem::CreateObstructionSphere(uint32_t gridIndex, float radius, glm::vec3 location)
    {
        NodeGrid* grid = m_NodeGrids[gridIndex].get();
        uint32_t sphereIndex = grid->m_ObstructionSpheres.CreateObstructionSphere(radius, location);

        // Set nodes within the sphere to Obstructions
        UpdateObstructionSphere(gridIndex, sphereIndex, radius, location);
        return sphereIndex;
    }

    void NodeGridSystem::DeleteObstructionSphere(uint32_t gridIndex, uint32_t sphereIndex)
    {
        NodeGrid* grid = m_NodeGrids[gridIndex].get();
        std::vector<int> nodes = grid->m_ObstructionSpheres.GetObstructionSphereNodes(sphereIndex);

        ObstructionUpdates update;
        update.m_ObstructionSpheres.push_back(sphereIndex);
        update.m_PotensiallyFalseObstructionNodes = nodes;
        m_ObstructionUpdates.push_back(update);
        //for (const auto it : nodes)
            //m_PotentiallyFalseObstructions.push_back(it);
        grid->m_ObstructionSpheres.EraseSphere(sphereIndex);
    }   

    void NodeGridSystem::UpdateObstructionSphere(uint32_t gridIndex, uint32_t sphereIndex, float radius, glm::vec3 location)
    {
        //int intRadius = (int)(radius * PATH_FLOATTOINT);
        NodeGrid* grid = m_NodeGrids[gridIndex].get();


        // Create ObstructionUpdate and add nearby spheres to it
        ObstructionUpdates update;
        update.m_ObstructionSpheres.push_back(sphereIndex);
            // Add nearby obstruction spheres   // Missing spatial partitioning 
        grid->m_ObstructionSpheres.UpdateSphereData(sphereIndex, radius, location);
        const auto& sphereLocations = grid->m_ObstructionSpheres.GetSphereLocations();
        for (int i = 0; i < grid->m_ObstructionSpheres.GetSphereCount(); i++)
        {
            if (glm::length(location - sphereLocations[i]) < radius + 100.f && i != sphereIndex)
                update.m_ObstructionSpheres.push_back(i);
        }

        // Mark nodes in obstruction sphere as PotentiallyFalseObstructions
        auto& nodes = grid->m_ObstructionSpheres.GetObstructionSphereNodes(sphereIndex);
        for (const auto& it : update.m_ObstructionSpheres)
            for (const auto& it2 : grid->m_ObstructionSpheres.GetObstructionSphereNodes(it))
            {
                int mRadius = (int)(grid->m_ObstructionSpheres.GetSphereRadiuses()[it] * PATH_FLOATTOINT);
                int distance = (int)(glm::length(grid->m_ObstructionSpheres.GetSphereLocations()[it] - grid->m_NodeLocations->at(it2)) * PATH_FLOATTOINT);
                if (distance > mRadius)
                    update.AddNode_Potensial(it2);
            }
        nodes.clear();

        // Old version
        //for (size_t i{}; i < nodes.size(); i++)
        //{
        //    int distance = (int)(glm::length(location - grid->m_NodeLocations->at(i)) * PATH_FLOATTOINT);
        //    if (distance > intRadius) {
        //        if (std::find(m_PotentiallyFalseObstructions.begin(), m_PotentiallyFalseObstructions.end(), i) == m_PotentiallyFalseObstructions.end())
        //            m_PotentiallyFalseObstructions.push_back(nodes[i]);
        //    }
        //}
        
        // Remove Nodes outside of obstruction sphere

        // Set nodes within the sphere to Obstructions
        //for (size_t i{}; i < grid->m_NodeLocations->size(); i++)
        //{
        //    int distance = (int)(glm::length(location - grid->m_NodeLocations->at(i)) * PATH_FLOATTOINT);
        //    if (distance <= intRadius) 
        //    {
        //        grid->m_NodeObstructionStatus->at(i) = true;
        //        nodes.push_back(i);
        //        //update.m_CurrentSphereNodes.push_back(i);
        //        update.AddNode_Current(i);
        //    }
        //}

        for (size_t i{}; i < grid->m_NodeLocations->size(); i++)
            for (const auto& it : update.m_ObstructionSpheres)
                {
                    int mRadius = (int)(grid->m_ObstructionSpheres.GetSphereRadiuses()[it] * PATH_FLOATTOINT);
                    int distance = (int)(glm::length(grid->m_ObstructionSpheres.GetSphereLocations()[it] - grid->m_NodeLocations->at(i)) * PATH_FLOATTOINT);
                    if (distance <= mRadius)
                    {
                        grid->m_NodeObstructionStatus->at(i) = true;
                        grid->m_ObstructionSpheres.GetObstructionSphereNodes(it).push_back(i);
                        update.AddNode_Current(i);
                    }
                }

        m_ObstructionUpdates.push_back(update);
    }

    void NodeGridSystem::UpdateFalseObstructionNodes(uint32_t gridIndex)
    {
        if (m_ObstructionUpdates.size() == 0) return;
        for (auto& it : m_ObstructionUpdates)
        {
            if (it.m_PotensiallyFalseObstructionNodes.size() == 0) continue;
            // Sammenlign med PotentiallyFalse og Current
            for (const auto& current : it.m_CurrentSphereNodes)
            {
                for (int potfalse = 0; potfalse < it.m_PotensiallyFalseObstructionNodes.size(); potfalse++)
                {
                    if (it.m_PotensiallyFalseObstructionNodes.size() != 0) break;
                    if (it.m_PotensiallyFalseObstructionNodes[potfalse] == current)
                    {
                        it.m_PotensiallyFalseObstructionNodes.erase(std::next(it.m_PotensiallyFalseObstructionNodes.begin(), potfalse));
                        potfalse--;
                        if (potfalse < 0) potfalse = 0;
                    }
                }
            }
        }
        // Set false nodes to false
        for (auto& it : m_ObstructionUpdates)
            for (auto& pot : it.m_PotensiallyFalseObstructionNodes)
                m_NodeGrids[gridIndex]->m_NodeObstructionStatus->at(pot) = false;
        m_ObstructionUpdates.clear();

        // Old Version
        //if (m_PotentiallyFalseObstructions.size() == 0) return;
        //auto& nodeGrids = m_NodeGrids[gridIndex];
        //std::vector<int> nodesToCheck;
        //
        //// Get every node index the obstruction spheres hold
        //for (const auto& it : nodeGrids->m_ObstructionSpheres.GetSphereCollections())
        //    for (const auto& it2 : it.m_nodes)
        //        nodesToCheck.push_back(it2);
        //
        //// Remove nodes 
        //int i{ 0 };
        //for (i = 0; i < m_PotentiallyFalseObstructions.size(); i++)
        //{
        //    for (const auto& it : nodesToCheck)
        //    {
        //        if (m_PotentiallyFalseObstructions.size() == 0) break;
        //        if (m_PotentiallyFalseObstructions[i] == it)
        //        {
        //            m_PotentiallyFalseObstructions.erase(std::next(m_PotentiallyFalseObstructions.begin(), i));
        //            i--;
        //            if (i < 0) i = 0;
        //        }
        //    }
        //}
        //
        //// Set node block status to false 
        //for (auto& obs : m_PotentiallyFalseObstructions) {
        //    m_NodeGrids[gridIndex]->m_NodeObstructionStatus->at(obs) = false;
        //}
        //m_PotentiallyFalseObstructions.clear();
    }

    void NodeGridSystem::CreateGridAtLocation(glm::vec3 location, glm::vec3 extent, int resolution)
    {
        int node{};
        m_NodeGrids.push_back(std::make_shared<NodeGrid>(location, extent, extent.x * resolution * extent.z * resolution));
        int GridIndex = m_NodeGrids.size() - 1;
        auto& grid = m_NodeGrids[GridIndex];

        int extent_X = (int)(extent.x);
        int extent_Z = (int)(extent.z);
        
        float xzRatio = 2.f * ((float)extent_X / (float)extent_Z);
        float step = 1.f / (float)resolution;

        for (float x{}; x < extent_X; x += step)
        {
            for (float z{}; z < extent_Z; z+=step)
            {
                glm::vec3 loc = glm::vec3(location.x + (x) - ((float)extent_X/2.f), 0, location.z + (z) - ((float)extent_Z/2.f));
                grid->AddNode(loc);
            }
        }
        int ext_X = extent_X * resolution;
        int ext_Z = extent_Z * resolution;
        for (int x{}; x < ext_X; x++)
        {
            for (int z{}; z < ext_Z; z++)
            {
                node = (x * ext_Z) + z;

                int a = (x * ext_Z) + z+1;
                int b = ((x+1) * ext_Z) + z;
                int c = (x * ext_Z) + z-1 ;
                int d = ((x-1) * ext_Z) + z;
                // Diagonal nodes
                int e = b + 1;
                int f = b - 1;
                int g = d - 1;
                int h = d + 1;

                // CORNERS  ------------------------
                    //Top Left
                if (c<0 && d<0) {
                    grid->ConnectNode(node, a);
                    grid->ConnectNode(node, b);
                    grid->ConnectNode(node, e);
                    continue;
                }
                    //Top Right
                if (x == ext_X -1 && z == 0) {
                    grid->ConnectNode(node, a);
                    grid->ConnectNode(node, d);
                    grid->ConnectNode(node, h);
                    continue;
                }
                    // Bottom Left
                if (x == 0 && z == ext_Z -1) {
                    grid->ConnectNode(node, b);
                    grid->ConnectNode(node, c);
                    grid->ConnectNode(node, f);
                    continue;
                }
                    // Bottom Right
                if (x == ext_X -1 && z == ext_Z -1) {
                    grid->ConnectNode(node, c);
                    grid->ConnectNode(node, d);
                    grid->ConnectNode(node, g);
                    continue;
                }

                // EDGES    ------------------------
                    // Top
                if (z == 0) {
                    grid->ConnectNode(node, a);
                    grid->ConnectNode(node, b);
                    grid->ConnectNode(node, d);
                    grid->ConnectNode(node, e);
                    grid->ConnectNode(node, h);
                    continue;
                }
                    // Left
                if (x == 0) {
                    grid->ConnectNode(node, a);
                    grid->ConnectNode(node, b);
                    grid->ConnectNode(node, c);
                    grid->ConnectNode(node, f);
                    grid->ConnectNode(node, e);
                    continue;
                }
                    // Bottom
                if (z == ext_Z -1) {
                    grid->ConnectNode(node, b);
                    grid->ConnectNode(node, c);
                    grid->ConnectNode(node, d);
                    grid->ConnectNode(node, f);
                    grid->ConnectNode(node, g);
                    continue;
                }
                    // Right
                if (x == ext_X -1) {
                    grid->ConnectNode(node, a);
                    grid->ConnectNode(node, c);
                    grid->ConnectNode(node, d);
                    grid->ConnectNode(node, g);
                    grid->ConnectNode(node, h);
                    continue;
                }

                // NO SPECIAL CASE  ------------------
                grid->ConnectNode(node, a);
                grid->ConnectNode(node, b);
                grid->ConnectNode(node, c);
                grid->ConnectNode(node, d);

                grid->ConnectNode(node, e);
                grid->ConnectNode(node, f);
                grid->ConnectNode(node, g);
                grid->ConnectNode(node, h);
            }
        }
    }

    //void NodeGridSystem::GenerateNodeNamesForGrid(std::shared_ptr<NodeGrid> grid)
    //{
    //    std::string baseName = "PNode ";
    //    uint32_t i{};
    //    for (auto& it : grid->m_Nodes) {
    //        i++;
    //        std::string name = baseName + std::to_string(i);
    //        it->m_Data->m_Name = name;
    //    }
    //}

}
