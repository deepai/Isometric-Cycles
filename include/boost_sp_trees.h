#ifndef _BOOST_SP_TREE
#define _BOOST_SP_TREE

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <queue>
#include <vector>
#include <utility>
#include <cmath>
#include <climits>

using namespace std;
using namespace boost;

struct compare_pair {

	bool operator()(std::pair<int, int> &a, std::pair<int, int> &b) {
		return (a.second > b.second);
	}
};

template <class Graph,class Vertex,class Edge_Weight_Array>
struct boost_sp_tree
{
	Graph &input_graph;
	Edge_Weight_Array &edge_weights;

	int num_nodes, count_edges;

	Vertex root;
	vector<Vertex> Parent;
	vector<int> D;
	vector<int> S;
	vector<int> Min_node;
	vector<bool> is_tree_edge;

	boost_sp_tree(Vertex root, Graph &G, Edge_Weight_Array &edge_weights) : root(root), input_graph(G) ,num_nodes(num_vertices(G)), count_edges(num_edges(G)), edge_weights(edge_weights)
	{
		Parent.resize(num_nodes);
		D.resize(num_nodes, INT_MAX);
		S.resize(num_nodes, -1);
		Min_node.resize(num_nodes);
		is_tree_edge.resize(count_edges);
		edge_weights = get(edge_weight, G);
	}

	int boost_calculate_sp();
	void calculate_lca(int existing_parent, int new_parent, int &min_existing, int &min_new);
};

#include "boost_sp_trees.hpp"

#endif