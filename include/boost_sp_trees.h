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

template <class Vertex,class Edge_Iterator>
struct boost_cycle
{
	Vertex root;
	int edge_weight;
	Edge_Iterator edge;

	boost_cycle(Vertex root, Edge_Iterator iter,int weight) : root(root), edge(iter), edge_weight(weight)
	{}

	bool operator<(const boost_cycle &rhs) const
	{
		return edge_weight < rhs.edge_weight;
	}

	boost_cycle(){}
};

template <class Graph,class Vertex,class Edge_Weight_Array, class Edge_Index_Array>
struct boost_sp_tree
{
	Graph &input_graph;
	Edge_Weight_Array &edge_weights;
	Edge_Index_Array &edge_indexes;

	int num_nodes, count_edges;

	Vertex root;
	vector<Vertex> Parent;
	vector<int> D;
	vector<int> S;
	vector<int> Min_node;
	vector<bool> is_tree_edge;

	typedef typename graph_traits<Graph>::edge_iterator Edge_Iterator;

	boost_sp_tree(Vertex root, Graph &G, Edge_Weight_Array &edge_weights, Edge_Index_Array &edge_indexes) 
		: root(root), input_graph(G) ,num_nodes(num_vertices(G)),
		  count_edges(num_edges(G)), edge_weights(edge_weights), edge_indexes(edge_indexes) 
	{
		Parent.resize(num_nodes);
		D.resize(num_nodes, INT_MAX);
		S.resize(num_nodes, -1);
		Min_node.resize(num_nodes);
		is_tree_edge.resize(count_edges, false);
		edge_weights = get(edge_weight, G);
	}

	int boost_calculate_sp();
	void calculate_lca(int existing_parent, int new_parent, int &min_existing, int &min_new);
	
	inline bool is_cycle(Vertex U, Vertex V);
	
	boost_cycle<Vertex,Edge_Iterator> get_cycle(Edge_Iterator &iter, Vertex U, Vertex V)
	{
		boost_cycle<Vertex,Edge_Iterator> cycle_b(root, iter, D[U] + D[V] + edge_weights[*iter]);
		return cycle_b;
	}
};

#include "boost_sp_trees.hpp"

#endif