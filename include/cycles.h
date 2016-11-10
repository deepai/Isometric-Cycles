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

	int cycle_index;

	cycle(){};

	cycle(int root, unsigned offset, unsigned total_weight)
	{
		this->root = root;
		this->edge_offset = offset;
		this->total_weight = total_weight;
	}

	static cycle* binary_search(std::vector<cycle*> &data, int start, int end, unsigned edge_offset)
	{
		if(data.size() == 0)
			return NULL;

		int mid;

		while(start <= end)
		{
			mid = start + (end - start)/2;
			if(data[mid]->edge_offset == edge_offset)
			{
				return data[mid];
			}
			else if(data[mid]->edge_offset < edge_offset)
			{
				start = mid + 1;
			}
			else
				end = mid - 1;
		}
		return NULL;
	}

	inline void set_index(int c_index)
	{
		cycle_index = c_index;
	}
};

struct compare_cycle
{
	bool operator()(cycle *lhs, cycle *rhs) const
	{
		return lhs->edge_offset < rhs->edge_offset;
	}
};

// template <class ForwardIterator, class T>
// int binary_search (ForwardIterator first, ForwardIterator last, const T& val, compare_cycle comp)
// {
// 	ForwardIterator result = std::lower_bound(first,last,val,comp());
// 	if(result!=last && !(val<*result))
// 		return -1;
// 	else
// 		return (int)(result - first);
// }

#endif