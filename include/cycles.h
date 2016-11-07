#ifndef _H_CYCLES
#define _H_CYCLES

#include "shortest_path_trees.h"

struct cycle
{
	shortest_path_tree *tree;

	unsigned edge_offset;
	unsigned total_weight;

	cycle(){};

	cycle(shortest_path_tree *curr_tree, unsigned offset, unsigned total_weight)
	{
		tree = curr_tree;
		edge_offset = offset;
		this->total_weight = total_weight;
	}
};

struct compare_cycle
{
	bool operator()(cycle *lhs, cycle *rhs) const
	{
		return lhs->edge_offset < rhs->edge_offset;
	}
};


#endif