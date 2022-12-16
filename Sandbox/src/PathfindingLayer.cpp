#include "PathfindingLayer.h"

//void PathfindingLayer::NodePicker(const char* title, int (&selected)[100], int& selectlocation, std::shared_ptr<Engine::PNode>& node)
//{
//	if (ImGui::TreeNode(title))
//	{
//		for (int y = 0; y < 10; y++)
//			for (int x = 0; x < 10; x++)
//			{
//				if (x > 0)
//					ImGui::SameLine();
//				int location = x * 10 + y;
//				std::string name = Engine::Pathfinder::m_Nodes[location]->m_name;
//				name.insert(5, "\n");
//				if (ImGui::Selectable(name.c_str(), selected[location] != 0, 0, ImVec2(50, 50)))
//				{
//					selected[location] = 1;
//					if (location != selectlocation)
//						selected[selectlocation] = 0;
//					selectlocation = location;
//					node = Engine::Pathfinder::m_Nodes[location];
//				}
//			}
//		ImGui::TreePop();
//	}
//}
//
//void PathfindingLayer::SelectableColor(ImVec2 selectable, ImU32 color)
//{
//	auto& style = ImGui::GetStyle();
//	ImVec2 min = style.FramePadding;
//	ImVec2 pos = ImGui::GetWindowPos();
//	ImVec2 max = selectable + pos;
//	ImGui::GetWindowDrawList()->AddRectFilled(pos + min, max, IM_COL32(255, 0, 0, 255));
//}
