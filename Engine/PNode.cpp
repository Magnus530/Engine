#include "epch.h"
#include "PNode.h"

namespace Pathfinding {

    PNode::PNode()
    {
    }

    PNode::~PNode()
    {
    }

    int PNode::GetInternalValue(PNode* target)
    {
        return (int)(mPosition - target->mPosition).length() * floatConvert;
    }

    PathSearch PNode::FindNextNode(PNode* nextnode, PPath* path, PNode* target)
    {
        if (this == target){
            nextnode = target;
            return PathSearch::Found;
        }

        int Final{ INT_MAX };
        int Value;
        auto paths = mConnectedPaths;
        for (size_t i{}; i < mConnectedPaths.size(); i++)
        {
            PNode* Node = paths[i]->GetOtherNode(this);
            if (Node == target) 
            {
                nextnode = Node;
                break;
            }

            Value = paths[i]->Length + Node->GetInternalValue(target);

            if (Value < Final && i) {
                Final = Value;
                nextnode = Node;
                if (path->ContainsNode(this)) { continue; }
                path->AddPath(Node, paths[i], Value);
            }
        }

        return PathSearch::Search;
    }

    bool FindPath(PPath* Path, PNode* Start, PNode* Target)
    {
        PNode* current{ Start };
        PNode* nextnode{ nullptr };
        PLine* line{ nullptr };
        PathSearch Searchtype = PathSearch::Start;

        while (true)
        {
            Searchtype = current->FindNextNode(nextnode, Path, Target);
            current = nextnode;
            if (Searchtype == PathSearch::Found){ return true; }
            // else if cannot be found within the node grid - return false;
        }
    }

}