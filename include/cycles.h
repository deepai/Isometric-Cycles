#ifndef _H_CYCLES
#define _H_CYCLES

#include "CsrGraphMulti.h"
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

	void print_cycle(int index, csr_multi_graph &graph, shortest_path_tree &tree)
	{
		int row = graph.rows->at(edge_offset);
		int col = graph.columns->at(edge_offset);

		printf("%d) cycle: root =  %d, weight = %u, %d - %d, ",index, root + 1, total_weight, row + 1, col + 1);

		while(row != root)
		{
			printf("%d - %d, ",tree.parent[row] + 1, row + 1);
			row = tree.parent[row];
		}

		while(col != root)
		{
			printf("%d - %d, ",tree.parent[col] + 1, col + 1);
			col = tree.parent[col];
		}
		printf("\n");
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

	static cycle* get_cycle_info(shortest_path_tree &tree, int edge_offset)
	{
		int row = tree.parent_graph.rows->at(edge_offset);
		int col = tree.parent_graph.columns->at(edge_offset);

		#ifdef PRINT
			printf("Cycle = %d, %d - %d\n",tree.root_node + 1, row + 1, col + 1);
			printf("%d %d %d %d\n", tree.minimum_node_in_path[row] + 1, tree.minimum_node_in_path[col] + 1, tree.S_value[row] + 1, tree.S_value[col] + 1 );
		#endif

		if(tree.minimum_node_in_path[row] == tree.root_node && tree.minimum_node_in_path[col] == tree.root_node
			&& tree.S_value[row] != tree.S_value[col] && tree.S_value[row] != -1 && tree.S_value[col] != -1)
		{
			cycle* c = new cycle(tree.root_node, edge_offset, tree.distance[row] + tree.distance[col] + tree.parent_graph.weights->at(edge_offset));
			c->S_value_row = tree.S_value_edge[row];
			c->S_value_col = tree.S_value_edge[col];

			#ifdef PRINT
				printf("Cycle Satisfying cases = %d, %d - %d\n",tree.root_node + 1, row + 1, col + 1);
			#endif

			return c;
		}
		else
			return NULL;
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