#include "epch.h"
#include "PNode.h"

namespace Engine {

    //int PNode::GetDistanceToNode(PNode* target)
    //{
    //    return (int)(glm::length(m_Position - target->m_Position) * floatConvert);
    //}

    //void PNode::InitValues(PNode* start, PNode* target)
    //{
    //    H = GetDistanceToNode(target);
    //    F = G + H;
    //}

    //void PNode::SetBlock(bool b)
    //{
    //    if (b) {
    //        NodeType = ENodeType::NT_Block;
    //    }
    //    else {
    //        NodeType = ENodeType::NT_None;
    //    }
    //}

    //namespace Pathfinder {

    //    glm::vec3 GetPositionAlongSpline(std::vector<glm::vec3>& splinepoints, float t)
    //    {
    //        if (splinepoints.size() == 0) {
    //            return glm::vec3();
    //        }
    //        int degree = m_SplineDegree;

    //        glm::vec3 pos{};
    //        for (size_t j{}; j < splinepoints.size(); j++) {
    //            pos += splinepoints[j] * Bid(t, j, degree);
    //        }
    //        return pos;
    //    }

    //    void MakeKnotVector(std::vector<glm::vec3> nodelocations)
    //    {
    //        m_Knotvector.clear();

    //        if (nodelocations.size() == 3) {
    //            m_Knotvector = { 0, 0, 0, 1, 1, 1 };
    //            return;
    //        }

    //        /* Knot vector */
    //        int knotpoints = nodelocations.size() + m_SplineDegree + 1;
    //        float part = (float)knotpoints - ((m_SplineDegree + 1) * 2);
    //        float step = (1 / (part + 1));

    //        for (int i{ 1 }; i <= knotpoints; i++)
    //        {
    //            if (i <= (m_SplineDegree + 1)) {
    //                m_Knotvector.push_back(0);
    //            }
    //            else if (i > knotpoints - (m_SplineDegree + 1)) {
    //                m_Knotvector.push_back(1);
    //            }
    //            else {
    //                int j = i - (m_SplineDegree + 1);
    //                float knot = step * j;
    //                m_Knotvector.push_back(knot);
    //            }
    //        }
    //    }

    //    glm::vec3 GetPositionofNode(uint32_t index)
    //    {
    //        if (index > m_Nodes.size() + 1)
    //            return glm::vec3();
    //        return m_Nodes[index]->m_Position;
    //    }

    //    std::shared_ptr<PNode> GetNodeAtIndex(uint32_t index)
    //    {
    //        if (index > m_Nodes.size() + 1)
    //            return nullptr;
    //        return m_Nodes[index];
    //    }

    //    std::shared_ptr<PNode> GetNodeClosestToPosition(glm::vec3 position)
    //    {
    //        float length = glm::length(m_Nodes[0]->m_Position - position);
    //        std::shared_ptr<PNode> node = m_Nodes[0];

    //        for (size_t i{}; i < m_NodeLocations.size(); i++)
    //        {
    //            float l = glm::length(m_NodeLocations[i] - position);
    //            if (l < length) {
    //                length = l;
    //                node = m_Nodes[i];
    //            }
    //        }
    //        return node;
    //    }

    //    std::vector<PNode*> FindPath(std::shared_ptr<PNode> start, PNode* end)
    //    {
    //        return std::vector<PNode*>();
    //    }

    //    float Bid(float t, int it, int d)
    //    {
    //        if (d == 0) {
    //            if (m_Knotvector[it] <= t && t < m_Knotvector[it + 1]) {
    //                return 1.f;
    //            }
    //            return 0.0f;
    //        }
    //        return (Wid(t, it, d) * Bid(t, it, d - 1)) + ((1 - Wid(t, it + 1, d)) * Bid(t, it + 1, d - 1));

    //    }

    //    float Wid(float t, int it, int d)
    //    {
    //        if (m_Knotvector[it] < m_Knotvector[it + d]) {
    //            return (t - m_Knotvector[it]) / (m_Knotvector[it + d] - m_Knotvector[it]);
    //        }
    //        return 0.0f;
    //    }
    //    
    //    //std::vector<PNode*> FindPath(std::shared_ptr<PNode> start, PNode* end)
    //    std::vector<PNode*> FindPath(std::shared_ptr<PNode> start, std::shared_ptr<PNode> end)
    //    {
    //        std::vector<PNode*> path;
    //        
    //        std::shared_ptr<PNode> closestNode{ start };     // If node is closed off, go to the closest one in, air length
    //        closestNode->InitValues(start.get(), end.get());
    //        std::vector<std::shared_ptr<PNode>> tosearch;
    //        tosearch.push_back(start);
    //        std::vector<std::shared_ptr<PNode>> processed;
    //    
    //        while (tosearch.size() > 0)
    //        {
    //            std::shared_ptr<PNode> current = tosearch[0];
    //            current->InitValues(start.get(), end.get());
    //    
    //            for (auto& it : tosearch)
    //            {
    //                if (it == current) { continue; }
    //                it->InitValues(start.get(), end.get());
    //                if (it->F < current->F || it->F == current->F && it->H < current->H)
    //                    current = it;
    //            }
    //    
    //            /* Setting closest node */
    //            if (closestNode->GetDistanceToNode(end.get()) > current->GetDistanceToNode(end.get()))
    //                closestNode = current;
    //            
    //            /* FOUND PATH: Setting Path */
    //            if (current.get() == end.get()) {
    //                /* Går bakover fra Target til Start */
    //                PNode* currentTile = end.get();
    //                while (currentTile != start.get()) {
    //                    path.push_back(currentTile);
    //                    currentTile = currentTile->m_Connection.get();
    //                }
    //                return path;
    //            }
    //    
    //            /* Removes current node from the to search list and adds it to processed */
    //            processed.push_back(current);
    //            std::vector<std::shared_ptr<PNode>>::iterator it = std::find(tosearch.begin(), tosearch.end(), current);
    //            if (it != tosearch.end()) {
    //                tosearch.erase(it);
    //            }
    //    
    //            /* Processes current node */
    //            for (size_t t{}; t < current->mConnectedNodes.size(); t++)
    //            {
    //                std::shared_ptr<PNode> neighbor = current->mConnectedNodes[t];
    //                auto it = std::find(tosearch.begin(), tosearch.end(), neighbor);
    //                bool inSearch = (it != tosearch.end());
    //    
    //                if (neighbor->IsBlock()) { continue; }
    //    
    //                it = std::find(processed.begin(), processed.end(), neighbor);
    //                if (it != processed.end()) { continue; }
    //    
    //                int CostToNeighbor = current->G + current->GetDistanceToNode(neighbor.get());
    //    
    //                if (!inSearch || CostToNeighbor < neighbor->G)
    //                {
    //                    neighbor->SetG(CostToNeighbor);
    //                    neighbor->m_Connection = current;
    //    
    //                    if (!inSearch) {
    //                        neighbor->SetH(end.get());
    //                        tosearch.push_back(neighbor);
    //                    }
    //                }
    //            }
    //        }
    //        /* If a path was not found, find the path to the closestNode */
    //        path = FindPath(start, closestNode);
    //        return path;
    //    }
    //}
}