/*
This file is part of Circuit Engine.
 
Circuit Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
 
Circuit Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with Circuit Engine.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------
	BBNode.h :  The atomic unit of BreadBoard class
				to memorize and check the connections between
				Circuit Elements.
*/

#pragma once
#include "Gate.h"

class BBNode
{
	Gate *gate_info;
	int node_map_index; // -1 means it's not plugged to any hole on BreadBoard.
    BBNode *next_hole;

public:

	BBNode();
	void SetGateInfo(Gate *new_gate_info);
	void SetNextHole(BBNode *new_next_hole);
	void SetNodeMapIndex(int new_node_map_index);
	Gate *GetGateInfo(void);
	BBNode *GetNextHole(void);
	int GetNodeMapIndex(void);
};

BBNode::BBNode()
{
	gate_info = NULL;
	next_hole = NULL;
	node_map_index = -1;
}
void BBNode::SetGateInfo(Gate *new_gate_info)
{
    gate_info = new_gate_info;
}
void BBNode::SetNextHole(BBNode *new_next_hole)
{
	next_hole = new_next_hole;
}
void BBNode::SetNodeMapIndex(int new_node_map_index)
{
    node_map_index = new_node_map_index;
}
Gate *BBNode::GetGateInfo(void)
{
	return gate_info;
}
BBNode *BBNode::GetNextHole(void)
{
    return next_hole;
}
int BBNode::GetNodeMapIndex(void)
{
	return node_map_index;
}
