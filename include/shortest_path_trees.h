#ifndef __H_SP_TREE
#define __H_SP_TREE

#include <queue>
#include <vector>
#include <utility>
#include <cmath>

#include "CsrGraphMulti.h"

//This class is used to store the shortest path trees

struct compare_pair {
	bool operator()(std::pair<int, int> &a, std::pair<int, int> &b) {
		return (a.second > b.second);
	}
};

struct shortest_path_tree
{
	const int root_node;
	const int num_nodes;

	csr_multi_graph &parent_graph;

	std::vector<int> parent;
	std::vector<int> edge_offsets;
	std::vector<int> S_value;
	std::vector<int> S_value_edge;
	std::vector<int> minimum_node_in_path;
	std::vector<int> distance;

	shortest_path_tree(int n, int root, csr_multi_graph &parent_graph) : root_node(root), num_nodes(n), parent_graph(parent_graph)
	{
		parent.resize(num_nodes);
		distance.resize(num_nodes, -1);
		edge_offsets.resize(num_nodes);
		S_value.resize(num_nodes, -1);
		S_value_edge.resize(num_nodes, -1);
		minimum_node_in_path.resize(num_nodes);

		S_value[root] = -1;
		S_value_edge[root] = -1;
		minimum_node_in_path[root] = root;
	}

	int get_root_value()
	{
		return root_node;
	}

	void calculate_sp_tree();

	~shortest_path_tree()
	{
		parent.clear();
		edge_offsets.clear();
		S_value.clear();
		S_value_edge.clear();
		minimum_node_in_path.clear();
	}

};

#endif