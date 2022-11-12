#include "epch.h"
#include "PNode.h"

namespace Pathfinding {

    PNode::PNode()
    {
    }

    PNode::~PNode()
    {
    }

    int PNode::GetInternalValue(const PNode* target)
    {
        return (int)(mPosition - target->mPosition).length() * floatConvert;
    }

    PNode* PNode::FindNextNode(EPathSearch& SearchMode, PPath* Path, const PNode* Start, const PNode* Target)
    {
        if (this == Target){
            SearchMode = EPathSearch::Found;
            return this;
        }

        int Final{ INT_MAX };
        int Value{};
        PNode* Node{ nullptr };
        PLine* Linepath{ nullptr };

        for (size_t i{}; i < mConnectedPaths.size(); i++)
        {
            PNode* tmpNode = mConnectedPaths[i]->GetOtherNode(this);
            if (Path->ContainsNode(tmpNode)) { continue; }
            
            if (tmpNode == Target) {
                SearchMode = EPathSearch::Found;
                return tmpNode;
            }

            Value = mConnectedPaths[i]->Length + tmpNode->GetInternalValue(Target);
            if (Value < Final) {
                Final = Value;
                Node = tmpNode;
                Path->AddPath(tmpNode, mConnectedPaths[i], Value);
                SearchMode = EPathSearch::Search;
            }
        }
        if (Node) {
            Path->AddPath(Node, Linepath, Value);
            Node->FindNextNode(SearchMode, Path, Start, Target);
        }
        
        return Node;
    }

    bool FindPath(PPath* Path, PNode* Start, const PNode* Target)
    {
        if (!Start || !Target) { return false; }

        //PNode* current{ Start };
        PNode* nextnode{ nullptr };
        PLine* line{ nullptr };
        EPathSearch Searchtype = EPathSearch::Start;

        nextnode = Start->FindNextNode(Searchtype, Path, Start, Target);
        
        if (Searchtype == EPathSearch::Found){ return true; }
        return false;
        // else if cannot be found within the node grid - return false;
    }

}