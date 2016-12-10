#ifndef _BOOST_SP_TREE
#define _BOOST_SP_TREE

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <vector>

using namespace std;
using namespace boost;

template<class Vertex>
struct SP_Info
{
	Vertex root;
	vector<Vertex> Parent;
	vector<int> D;
	vector<int> S;
	vector<int> Min_node;
	vector<bool> is_tree_edge;

	SP_Info(Vertex root, int num_nodes, int num_edges) : root(root)
	{
		Parent.resize(num_nodes);
		D.resize(num_nodes);
		S.resize(num_nodes);
		Min_node.resize(num_nodes);
		is_tree_edge.resize(num_edges);
	}

	~SP_Info()
	{
		Parent.clear();
		D.clear();
		S.clear();
		Min_node.clear();
		is_tree_edge.clear();
	}
};
template<class Vertex>
struct SP_VISITOR : public default_dijkstra_visitor
{
	SP_Info<Vertex> &info;
	SP_VISITOR(SP_Info<Vertex> &info) : info(info){}
};

template <class Graph,class Vertex>
struct boost_sp_tree
{
	Graph &input_graph;
	SP_Info<Vertex> info;

	SP_VISITOR<Vertex> vis;

	boost_sp_tree(Vertex root, Graph &G) : input_graph(G) , info(root, num_vertices(G), num_edges(G)) , vis(info)
	{

	}

	void boost_calculate_sp();
};

#include "boost_sp_trees.hpp"

#endif