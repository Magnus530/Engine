#include "epch.h"
#include "PathfindingSystem.h"
#include "Engine/Scene/Scene.h"

namespace Engine {
    //**********************************************************************************************************************************************************************************************//
    //************************************************************************ PATHFINDING SYSTEM **********************************************************************************//
    //**********************************************************************************************************************************************************************************************//
	void PathfindingSystem::FindPath(Scene* scene, PathfindingComponent& comp, const glm::vec3 currentPosition)
	{
        //** Init Pathfinding
        const bool NoMovement = comp.m_StartNode == comp.m_TargetNode;
        if (NoMovement) return;
        auto grid = scene->m_PathfindingNodeGrid.get();
        if (comp.bIsMovingAlongPath)
            comp.m_StartNode = PathfindingSystem::GetNodeClosestToPosition(scene, currentPosition);
        comp.bIsMovingAlongPath = true;


        // Find path to target, Component stores vector<int>pathIndex 
        comp.m_CurrentPath = FindPathAStar(scene, comp.m_StartNode, comp.m_TargetNode, comp.m_IntermediateTargetNode, &comp.bIsObstructed);
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


        // Get length of spline path for accurate movement speed speed through spline
        comp.m_SplineLength = 0.f;
        glm::vec3 prevPos = currentPosition;
        for (float t{}; t < 1.f; t += 0.05)
        {
            glm::vec3 pos = BSplineCreator::GetLocationAlongSpline(*comp.m_SplinePath, t);

            float l = glm::length(pos - prevPos);
            comp.m_SplineLength += l;

            prevPos = BSplineCreator::GetLocationAlongSpline(*comp.m_SplinePath, t);
        }
	}





    void PathfindingSystem::MoveAlongPath(Scene* scene, PathfindingComponent& pathfinder, TransformComponent& transform, float deltatime)
    {
        if (!pathfinder.bIsMovingAlongPath) return;
        if (pathfinder.bReachedTarget) return;

        float& t = pathfinder.m_SplineMovement;
        float speed = pathfinder.m_EntityPathSpeed / pathfinder.m_SplineLength;
        if (speed <= 0.f) return;

        if (t < 0.99f) {
            t = std::clamp<float>(t += deltatime * speed, 0.f, 0.99f);
        }
        else {  // Reached end of path
            t = 0.f;
            pathfinder.m_StartNode = pathfinder.m_TargetNode;
            pathfinder.bReachedTarget = true;

            if (pathfinder.bPatrolling) {
                PatrolUpdate(scene, pathfinder, transform.GetLocation());
                return;
            }
        }
        
        if (!pathfinder.bReachedTarget) 
        {
            glm::vec3 previousPosition = transform.GetLocation();
            glm::vec3 pos = BSplineCreator::GetLocationAlongSpline(*pathfinder.m_SplinePath, t);

            glm::vec3 direction = pos - previousPosition;
            direction.y = 0.f;

            TransformSystem::SetWorldPosition(transform, pos + glm::vec3(0,0.5f,0));   // Manually adding extra height
            TransformSystem::RotateToDirectionVector(transform, glm::normalize(direction));
        }
    }

    void PathfindingSystem::ResumeMovementAlongPath(Scene* scene, PathfindingComponent& pathfinder, const glm::vec3 currentPosition)
    {
        pathfinder.bIsMovingAlongPath = true;
        pathfinder.m_StartNode = GetNodeClosestToPosition(scene, currentPosition);
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
        pathfinder.m_StartNode = PathfindingSystem::GetNodeClosestToPosition(scene, currentPosition);  // Kan også finne ut av hvilken punkt den er nærmest, og starte der. For å kunne restarte patrol
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
        pathfinder.m_TargetNode = PathfindingSystem::GetNodeClosestToPosition(scene, pathfinder.m_PatrolPath[pathfinder.m_CurrentPatrolPoint]);
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
        pathfinder.m_StartNode = GetNodeClosestToPosition(scene, currentPosition);

        pathfinder.m_CurrentPatrolPoint = std::clamp<int>(pathfinder.m_CurrentPatrolPoint, 0, pathfinder.m_PatrolPath.size() - 1);
        pathfinder.m_TargetNode = PathfindingSystem::GetNodeClosestToPosition(scene, pathfinder.m_PatrolPath[pathfinder.m_CurrentPatrolPoint]);
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

    int PathfindingSystem::GetNodeClosestToPosition(Scene* scene, glm::vec3 position)
    {
        auto grid = scene->m_PathfindingNodeGrid.get();
    
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

    std::vector<int> PathfindingSystem::FindPathAStar(Scene* scene, int startNode, int endNode, int& intermediateNode, bool* blocked)
    {
        std::vector<int> path;
        auto grid = scene->m_PathfindingNodeGrid.get();
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
        path = FindPathAStar(scene, startNode, closestNode, intermediateNode/*, &b*/);
        *blocked = true;
        intermediateNode = closestNode;
        return path;
    }

    //**********************************************************************************************************************************************************************************************//
    //************************************************************************* NODE GRID SYSTEM ***************************************************************************************************//
    //**********************************************************************************************************************************************************************************************//

    // Vector of Obstruction Updates. Cleared each frame after ObstructionUpdate
    static std::vector<ObstructionUpdates> m_ObstructionUpdates;

    glm::vec3 NodeGridSystem::GetNodeLocation(Scene* scene, int NodeIndex)
    {
        return scene->m_PathfindingNodeGrid->m_NodeLocations->at(NodeIndex);
    }

    uint32_t NodeGridSystem::CreateObstructionSphere(Scene* scene, float radius, glm::vec3 location)
    {
        auto grid = scene->m_PathfindingNodeGrid.get();
        uint32_t sphereIndex = grid->m_ObstructionSpheres.CreateObstructionSphere(radius, location);

        // Set nodes within the sphere to Obstructions
        for (int i = 0; i < grid->m_NodeLocations->size(); i++)
        { 
            int mRadius = (int)(radius * PATH_FLOATTOINT);
            int distance = (int)(glm::length(location - grid->m_NodeLocations->at(i)) * PATH_FLOATTOINT);
            if (distance <= mRadius)
            {
                grid->m_NodeObstructionStatus->at(i) = true;
                grid->m_ObstructionSpheres.GetObstructionSphereNodes(sphereIndex).push_back(i);
            }
        }
        return sphereIndex;
    }

    void NodeGridSystem::DeleteObstructionSphere(Scene* scene, uint32_t sphereIndex)
    {
        auto grid = scene->m_PathfindingNodeGrid.get();
        std::vector<int> nodes = grid->m_ObstructionSpheres.GetObstructionSphereNodes(sphereIndex);
        
        ObstructionUpdates update;
        update.m_ObstructionSpheres.push_back(sphereIndex);
            // Add nearby obstruction spheres   
        
        const auto& sphereLocations = grid->m_ObstructionSpheres.GetSphereLocations();
        for (int i = 0; i < grid->m_ObstructionSpheres.GetSphereCount(); i++)
            if (glm::length(sphereLocations[sphereIndex] - sphereLocations[i]) < grid->m_ObstructionSpheres.GetSphereRadiuses()[sphereIndex] + 100.f && i != sphereIndex)    // + 100.f is an arbitrary amount added to the radius, 
                update.m_ObstructionSpheres.push_back(i);                                             // to make sure the nearby Obstructionspheres are added.
        
        update.m_PotensiallyFalse = nodes;

        
        m_ObstructionUpdates.push_back(update);
        UpdateFalseObstructionNodes(scene);
        grid->m_ObstructionSpheres.EraseSphere(sphereIndex);
    }   

    void NodeGridSystem::UpdateObstructionSphere(Scene* scene, uint32_t sphereIndex, float radius, glm::vec3 location)
    {
        // Create ObstructionUpdate and add nearby spheres to it
            // Add nearby obstruction spheres   
        // Mark nodes in obstruction sphere as PotentiallyFalseObstructions
        // Re add all nodes that are within the radius of the ObstructionUpdates' ObstructionSpheres

        auto grid = scene->m_PathfindingNodeGrid.get();

        ObstructionUpdates update;
        update.m_ObstructionSpheres.push_back(sphereIndex);
        grid->m_ObstructionSpheres.UpdateSphereData(sphereIndex, radius, location);
        const auto& sphereLocations = grid->m_ObstructionSpheres.GetSphereLocations();
        for (int i = 0; i < grid->m_ObstructionSpheres.GetSphereCount(); i++)
            if (glm::length(location - sphereLocations[i]) < radius + 100.f && i != sphereIndex)    // + 100.f is an arbitrary amount added to the radius, 
                update.m_ObstructionSpheres.push_back(i);                                             // to make sure the nearby Obstructionspheres are added.

        auto& nodes = grid->m_ObstructionSpheres.GetObstructionSphereNodes(sphereIndex);
        for (const auto& it2 : grid->m_ObstructionSpheres.GetObstructionSphereNodes(sphereIndex))
        {
            int mRadius = (int)(grid->m_ObstructionSpheres.GetSphereRadiuses()[sphereIndex] * PATH_FLOATTOINT);
            int distance = (int)(glm::length(grid->m_ObstructionSpheres.GetSphereLocations()[sphereIndex] - grid->m_NodeLocations->at(it2)) * PATH_FLOATTOINT);
            if (distance > mRadius)
                update.AddNode_PotensiallyFalse(it2);
        }
        nodes.clear();

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

    void NodeGridSystem::UpdateFalseObstructionNodes(Scene* scene)
    {
        // Iterate through every ObstructionUpdate object
            // Iterate through current sphere nodes and compare with PotentiallyFalse
                // Iterate through the nodes of m_PotensiallyFalse
                    // Remove all nodes from m_PotensiallyFalse that exists within the m_CurrentSphereNodes vector
        // Every then now remaining within m_PotensiallyFalse is guaranteed to be false
        // Set obstructionstatus to every nodes remaining within m_PotensiallyFalse to false

        if (m_ObstructionUpdates.size() == 0) return;
        for (auto& it : m_ObstructionUpdates)
        {
            if (it.m_PotensiallyFalse.size() == 0) continue;
            for (const auto& current : it.m_CurrentSphereNodes)
            {
                for (int potfalse = 0; potfalse < it.m_PotensiallyFalse.size(); potfalse++)
                {
                    if (it.m_PotensiallyFalse.size() != 0) break;
                    if (it.m_PotensiallyFalse[potfalse] == current)
                    {
                        it.m_PotensiallyFalse.erase(std::next(it.m_PotensiallyFalse.begin(), potfalse));
                        potfalse--;
                        if (potfalse < 0) potfalse = 0;
                    }
                }
            }
        }
        for (auto& it : m_ObstructionUpdates)
            for (auto& pot : it.m_PotensiallyFalse)
                scene->m_PathfindingNodeGrid->m_NodeObstructionStatus->at(pot) = false;
        m_ObstructionUpdates.clear();
    }

    void NodeGridSystem::CreateGridAtLocation(Scene* scene, glm::vec3 location, glm::ivec3 extent, int resolution, bool bDebugRenderEnabled)
    {
        scene->m_PathfindingNodeGrid.reset(new NodeGrid(location, extent, resolution, extent.x * resolution * extent.z * resolution, bDebugRenderEnabled));

        auto grid = scene->m_PathfindingNodeGrid.get();
        int node{};

        int extent_X = (int)(extent.x);
        int extent_Z = (int)(extent.z);
        
        float xzRatio = 2.f * ((float)extent_X / (float)extent_Z);
        float step = 1.f / (float)resolution;

        // Creating Nodes at a given location in within the extent of the Grid 
        for (float x{}; x < extent_X; x += step)
            for (float z{}; z < extent_Z; z+=step)
            {
                glm::vec3 loc = glm::vec3(location.x + (x) - ((float)extent_X/2.f), 0, location.z + (z) - ((float)extent_Z/2.f));
                grid->AddNode(loc);
            }

        // Adding Node Neighbours 
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
