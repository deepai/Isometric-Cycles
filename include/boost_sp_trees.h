#ifndef _BOOST_SP_TREE
#define _BOOST_SP_TREE

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <queue>
#include <vector>
#include <utility>
#include <cmath>

using namespace std;
using namespace boost;

struct compare_pair {

	bool operator()(std::pair<int, int> &a, std::pair<int, int> &b) {
		return (a.second > b.second);
	}
};

template <class Graph,class Vertex>
struct boost_sp_tree
{
	Graph &input_graph;

	int num_nodes, count_edges;

	Vertex root;
	vector<Vertex> Parent;
	vector<int> D;
	vector<int> S;
	vector<int> Min_node;
	vector<bool> is_tree_edge;

	boost_sp_tree(Vertex root, Graph &G) : root(root), input_graph(G) ,  num_nodes(num_vertices(G)), count_edges(num_edges(G))
	{
		Parent.resize(num_nodes);
		D.resize(num_nodes);
		S.resize(num_nodes);
		Min_node.resize(num_nodes);
		is_tree_edge.resize(count_edges);
	}

	void boost_calculate_sp();
};

#include "boost_sp_trees.hpp"

#endif