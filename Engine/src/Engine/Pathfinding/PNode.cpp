#include "epch.h"
#include "PNode.h"

namespace Engine {

    PNode::PNode()
    {
    }

    PNode::PNode(glm::vec3 position)
        : m_Position{position}
    {
    }


    PNode::~PNode()
    {
    }

    int PNode::GetDistanceToNode(PNode* target)
    {
        return (int)(m_Position - target->m_Position).length() * floatConvert;
    }

    void PNode::InitValues(PNode* start, PNode* target)
    {
        H = GetDistanceToNode(target);
        F = G + H;
    }


    std::vector<PNode*> FindPath(PNode* start, PNode* end)
    {
        std::vector<PNode*> path;

        std::vector<PNode*> tosearch;
        tosearch.push_back(start);
        std::vector<PNode*> processed;

        while (tosearch.size() > 0)
        {
            PNode* current = tosearch[0];
            current->InitValues(start, end);
            
            for (auto& it : tosearch)
            {
                if (it == current) { continue; }
                it->InitValues(start, end);

                if (it->F < current->F || it->F == current->F && it->H < current->H)
                    current = it;

                if (current == end) {
                    /* Går bakover fra Target til Start */
                    PNode* currentTile = end;
                    while (currentTile != start) {
                        path.push_back(currentTile);
                        currentTile = currentTile->m_Connection;
                    }
                    return path;
                }

                /* Removes current node from the to search list and adds it to processed */
                processed.push_back(current);
                std::vector<PNode*>::iterator it = std::find(tosearch.begin(), tosearch.end(), current);
                if (it != tosearch.end()){
                    tosearch.erase(it);
                }

                /* Processes current node */
                for (size_t t{}; t < current->mConnectedNodes.size(); t++)
                {
                    PNode* neighbor = current->mConnectedNodes[t];
                    std::vector<PNode*>::iterator it = std::find(tosearch.begin(), tosearch.end(), neighbor);
                    bool inSearch = (it != tosearch.end());


                    if (neighbor->IsBlock()) { continue; }

                    it = std::find(processed.begin(), processed.end(), neighbor);
                    if (it != processed.end()) continue;

                    int CostToNeighbor = current->G + current->GetDistanceToNode(neighbor);

                    if (!inSearch || CostToNeighbor < neighbor->G)
                    {
                        neighbor->SetG(CostToNeighbor);
                        neighbor->m_Connection = current;

                        if (!inSearch) {
                            neighbor->SetH(end);
                            tosearch.push_back(neighbor);
                        }
                    }
                }
            }
        }

        return path;
    }

}