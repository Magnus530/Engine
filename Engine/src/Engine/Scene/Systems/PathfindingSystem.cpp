#include "epch.h"
#include "PathfindingSystem.h"
#include "Engine/Scene/Scene.h"

namespace Engine {
    //static std::vector<std::shared_ptr<NodeGrid>> m_NodeGrids;

    //************************************************************************ PATHFINDING SYSTEM **********************************************************************************//
	void PathfindingSystem::FindPath(Scene* scene, PathfindingComponent& comp, const glm::vec3 currentPosition)
	{
        //** Init Pathfinding
        const bool NoMovement = comp.m_StartNode == comp.m_TargetNode;
        if (NoMovement) return;

        auto& grid = scene->m_PathfindingNodeGrids[0];
        
        if (comp.bIsMovingAlongPath)
            comp.m_StartNode = PathfindingSystem::GetNodeClosestToPosition(scene, 0, currentPosition);
        comp.bIsMovingAlongPath = true;    // In init or FindPath?
        //*** END Init Pathfinding ***

        // Bit reduntant for pathfinding component to have array of node pointers, when it only uses their locations
        comp.m_CurrentPath = FindPathAStar(scene, 0, comp.m_StartNode, comp.m_TargetNode, comp.m_IntermediateTargetNode, &comp.bIsObstructed);
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

    void PathfindingSystem::PathMovement(Scene* scene, PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime)
    {
    }

    void PathfindingSystem::MoveAlongPath(Scene* scene, PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime)
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
                PatrolUpdate(scene, pathfinder, transform.GetPosition());
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

    void PathfindingSystem::ResumeMovementAlongPath(Scene* scene, PathfindingComponent& pathfinder, const glm::vec3 currentPosition)
    {
        pathfinder.bIsMovingAlongPath = true;
        pathfinder.m_StartNode = GetNodeClosestToPosition(scene, 0, currentPosition);
        FindPath(scene, pathfinder, currentPosition);
    }

    void PathfindingSystem::CancelMovementAlongPath(Scene* scene, PathfindingComponent& pathfinder)
    {
        pathfinder.bIsMovingAlongPath = false;
        pathfinder.m_CurrentPath.clear();
    }

    void PathfindingSystem::MoveAlongPatrol(Scene* scene, PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime)
    {
    }

    void PathfindingSystem::StartPatrol(Scene* scene, PathfindingComponent& pathfinder, const glm::vec3 currentPosition, PatrolType patroltype/* = PatrolType::Loop*/)
    {
        if (pathfinder.m_PatrolPath.size() < 2)
            return;

        // Called at start of patrol and each time entity reaches their target
            // Sets next point in patrolpath
        pathfinder.bPatrolling = true;
        pathfinder.m_PatrolType = patroltype;
        pathfinder.m_StartNode = PathfindingSystem::GetNodeClosestToPosition(scene, 0, currentPosition);  // Kan også finne ut av hvilken punkt den er nærmest, og starte der. For å kunne restarte patrol
        PatrolUpdate(scene, pathfinder, currentPosition);
    }

    void PathfindingSystem::PatrolUpdate(Scene* scene, PathfindingComponent& pathfinder, const glm::vec3 currentPosition)
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
            if (pathfinder.bReverse)
            {
                pathfinder.m_CurrentPatrolPoint--;
                if (pathfinder.m_CurrentPatrolPoint < 0)
                {
                    pathfinder.m_CurrentPatrolPoint += 2;
                    pathfinder.bReverse = false;
                    break;
                }
            }
            else
            {
                pathfinder.m_CurrentPatrolPoint++;
                if (pathfinder.m_CurrentPatrolPoint == pathfinder.m_PatrolPath.size())
                {
                    pathfinder.m_CurrentPatrolPoint -= 2;
                    pathfinder.bReverse = true;
                    break;
                }
            }
            break;
        }
        default:
            break;
        }

        pathfinder.m_CurrentPatrolPoint = std::clamp<int>(pathfinder.m_CurrentPatrolPoint, 0, pathfinder.m_PatrolPath.size() - 1);
        pathfinder.m_TargetNode = PathfindingSystem::GetNodeClosestToPosition(scene, 0, pathfinder.m_PatrolPath[pathfinder.m_CurrentPatrolPoint]);
        FindPath(scene, pathfinder, currentPosition - glm::vec3(0, 0.5f, 0));
    }

    void PathfindingSystem::PausePatrol(Scene* scene, PathfindingComponent& pathfinder, const glm::vec3 currentPosition)
    {
        pathfinder.bPatrolling = false;
        pathfinder.m_PatrolPauseLocation = currentPosition;
        CancelMovementAlongPath(scene, pathfinder);
    }

    void PathfindingSystem::ResumePatrol(Scene* scene, PathfindingComponent& pathfinder, const glm::vec3 currentPosition, PatrolType patroltype)
    {
        pathfinder.bPatrolling = true;
        pathfinder.bIsMovingAlongPath = true;
        pathfinder.m_PatrolType = patroltype;
        pathfinder.m_StartNode = GetNodeClosestToPosition(scene, 0, currentPosition);

        pathfinder.m_CurrentPatrolPoint = std::clamp<int>(pathfinder.m_CurrentPatrolPoint, 0, pathfinder.m_PatrolPath.size() - 1);
        pathfinder.m_TargetNode = PathfindingSystem::GetNodeClosestToPosition(scene, 0, pathfinder.m_PatrolPath[pathfinder.m_CurrentPatrolPoint]);
        FindPath(scene, pathfinder, currentPosition - glm::vec3(0, 0.5f, 0));
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
    int PathfindingSystem::GetNodeClosestToPosition(Scene* scene, uint32_t gridIndex, glm::vec3 position)
    {
        std::shared_ptr<NodeGrid> grid = NodeGridSystem::GetGridAtIndex(scene, gridIndex);
    
        float length = glm::length(grid->m_NodeLocations->at(0) - position);
        int nodeIndex{ 0 };
    
        for (size_t i{}; i < grid->m_NodeLocations->size(); i++)
        {
            float l = glm::length(grid->m_NodeLocations->at(i) - position);
            if (l < length) {
                length = l;
                nodeIndex = i;
            }
        }
        return nodeIndex;
    }

    std::vector<int> PathfindingSystem::FindPathAStar(Scene* scene, int gridIndex, int startNode, int endNode, int& intermediateNode, bool* blocked)
    {
        std::vector<int> path;
        auto& grid = scene->m_PathfindingNodeGrids[gridIndex];
        intermediateNode = -1;

        if (startNode == endNode)
            return path;


        int closestNode{ startNode };     // If node is closed off, go to the closest one in, air length
        grid->NodeInitValues(closestNode, endNode);
        std::vector<int> tosearch;
        tosearch.push_back(startNode);
        std::vector<int> processed;

        while (tosearch.size() > 0)
        {
            int current = tosearch[0];
            grid->NodeInitValues(current, endNode);

            /* Choosing node to process from the insearch array */
            for (auto& it : tosearch)
            {
                if (it == current) { continue; }
                grid->NodeInitValues(it, endNode);
                auto& d = grid->m_NodeDN->m_DistanceValues;
                if (d[it].F < d[current].F || d[it].F == d[current].F && d[it].H < d[current].H)
                    current = it;
            }

            /* Setting closest node */
            if (grid->GetNodeDistanceToNode(closestNode, endNode) > grid->GetNodeDistanceToNode(current, endNode))
                closestNode = current;

            /* FOUND PATH: Setting Path */
            if (current == endNode) {
                /* Går bakover fra Target til Start */
                int currentTile = endNode;
                while (currentTile != startNode) {
                    path.push_back(currentTile);
                    currentTile = grid->m_NodePathConnection->at(currentTile);
                }
                return path;
            }

            /* Removes the chosen node from tosearch and adds it to processed */
            processed.push_back(current);
            auto it = std::find(tosearch.begin(), tosearch.end(), current);
            if (it != tosearch.end()) {
                tosearch.erase(it);
            }

            /* Processes chosen node */
            size_t t{ 0 };
            for (t = 0; t < grid->m_NodeDN->m_NeighborIndices[current].size() - 1; t++)
            {
                int neighbor = grid->m_NodeDN->m_NeighborIndices[current][t];
                auto& it = std::find(tosearch.begin(), tosearch.end(), neighbor);
                bool inSearch = (it != tosearch.end());

                if (grid->m_NodeObstructionStatus->at(neighbor)) { continue; }

                it = std::find(processed.begin(), processed.end(), neighbor);
                if (it != processed.end()) { continue; }

                int CostToNeighbor = grid->m_NodeDN->m_DistanceValues[current].G + grid->GetNodeDistanceToNode(current, endNode);

                if (!inSearch || CostToNeighbor < grid->m_NodeDN->m_DistanceValues[neighbor].G)
                {
                    grid->m_NodeDN->m_DistanceValues[neighbor].G = CostToNeighbor;
                    grid->m_NodePathConnection->at(neighbor) = current;

                    if (!inSearch) {
                        grid->m_NodeDN->m_DistanceValues[neighbor].H = grid->GetNodeDistanceToNode(neighbor, endNode);
                        tosearch.push_back(neighbor);
                    }
                }
            }
        }
        /* If a path was not found, find the path to the closestNode */
        path = FindPathAStar(scene, gridIndex, startNode, closestNode, intermediateNode/*, &b*/);
        *blocked = true;
        intermediateNode = closestNode;
        return path;
    }

    //************************************************************************* NODE GRID SYSTEM ***************************************************************************************************//

    /* Nodes, that potentially, are no longer obstructions */
    static std::vector<ObstructionUpdates> m_ObstructionUpdates;

    std::shared_ptr<NodeGrid> NodeGridSystem::GetGridAtIndex(Scene* scene, uint32_t index)
    {
        return scene->m_PathfindingNodeGrids[0];
    }
    glm::vec3 NodeGridSystem::GetNodeLocation(Scene* scene, int gridIndex, int NodeIndex)
    {
        return scene->m_PathfindingNodeGrids[gridIndex]->m_NodeLocations->at(NodeIndex);
    }

    uint32_t NodeGridSystem::CreateObstructionSphere(Scene* scene, uint32_t gridIndex, float radius, glm::vec3 location)
    {
        NodeGrid* grid = scene->m_PathfindingNodeGrids[gridIndex].get();
        uint32_t sphereIndex = grid->m_ObstructionSpheres.CreateObstructionSphere(radius, location);

        // Set nodes within the sphere to Obstructions
        UpdateObstructionSphere(scene, gridIndex, sphereIndex, radius, location);
        return sphereIndex;
    }

    void NodeGridSystem::DeleteObstructionSphere(Scene* scene, uint32_t gridIndex, uint32_t sphereIndex)
    {
        NodeGrid* grid = scene->m_PathfindingNodeGrids[gridIndex].get();
        std::vector<int> nodes = grid->m_ObstructionSpheres.GetObstructionSphereNodes(sphereIndex);

        ObstructionUpdates update;
        update.m_ObstructionSpheres.push_back(sphereIndex);
        update.m_PotensiallyFalseObstructionNodes = nodes;
        m_ObstructionUpdates.push_back(update);
        //for (const auto it : nodes)
            //m_PotentiallyFalseObstructions.push_back(it);
        grid->m_ObstructionSpheres.EraseSphere(sphereIndex);
    }   

    void NodeGridSystem::UpdateObstructionSphere(Scene* scene, uint32_t gridIndex, uint32_t sphereIndex, float radius, glm::vec3 location)
    {
        //int intRadius = (int)(radius * PATH_FLOATTOINT);
        NodeGrid* grid = scene->m_PathfindingNodeGrids[gridIndex].get();


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

        // Re add all nodes within the ObstructionUpdates ObstructionSpheres
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

    void NodeGridSystem::UpdateFalseObstructionNodes(Scene* scene, uint32_t gridIndex)
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
                scene->m_PathfindingNodeGrids[gridIndex]->m_NodeObstructionStatus->at(pot) = false;
        m_ObstructionUpdates.clear();
    }

    void NodeGridSystem::CreateGridAtLocation(Scene* scene, glm::vec3 location, glm::ivec3 extent, int resolution, bool bDebugRenderEnabled)
    {
        auto& nodegrids = scene->m_PathfindingNodeGrids;
        nodegrids.push_back(std::make_shared<NodeGrid>(location, extent, resolution, extent.x * resolution * extent.z * resolution, bDebugRenderEnabled));
        int GridIndex = nodegrids.size() - 1;
        auto& grid = nodegrids[GridIndex];
        int node{};

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
}
