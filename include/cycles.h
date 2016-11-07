#ifndef _H_CYCLES
#define _H_CYCLES

#include "shortest_path_trees.h"

struct cycle
{
	int root;

	unsigned edge_offset;
	unsigned total_weight;

	int S_value_row;
	int S_value_col;

	cycle(){};

	cycle(int root, unsigned offset, unsigned total_weight)
	{
		this->root = root;
		this->edge_offset = offset;
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