#include <iostream>
// include <boost/graph/adjacency_list.hpp> // for customizable graphs
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "FileReader.h"
#include "FileWriter.h"

#include "boost_sp_trees.h"

using namespace std;
using namespace boost;

string input_graph;
string input_dual_graph;

typedef adjacency_list
< vecS,
  vecS,
  undirectedS,
  property<vertex_index_t, int>,
  property<edge_index_t, int,
  property<edge_weight_t,int> >
> 
Graph;

typedef graph_traits<Graph>::edge_descriptor Edge;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef property_map<Graph, edge_weight_t>::type Edge_Weight_Array;
typedef property_map<Graph, edge_index_t>::type Edge_Index_Array;
typedef graph_traits<Graph>::edge_iterator Edge_Iterator;

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		printf("First Argument is the Input Graph\n");
		printf("Second Argument is the Dual Graph\n");
		return 0;
	}
	input_graph = string(argv[1]);
	input_dual_graph = string(argv[2]);

	int num_nodes_G, num_edges_G;
	int num_nodes_dual_G, num_edges_dual_G;

	int src, dest, weight;
	FileReader file_in(input_graph.c_str());

	file_in.get_nodes_edges(num_nodes_G, num_edges_G);

	Graph G(num_nodes_G);
    Edge edge;
    Edge_Iterator ei, ei_end;

    Edge_Weight_Array edge_weights = get(edge_weight, G);

	for(int i=0; i < num_edges_G; i++){
		file_in.read_edge(src, dest, weight);
		edge = add_edge(src, dest, G).first;
        edge_weights[edge] = weight;
	}

	file_in.fileClose();

	FileReader file_dual_in(input_dual_graph.c_str());

	file_dual_in.get_nodes_edges(num_nodes_dual_G, num_edges_dual_G);

	Edge *edges_G = new Edge[num_edges_G]; //stores edges of G
	Edge *edges_dual_G = new Edge[num_edges_G]; //stores edges of dual_G

	int edge_id = 0, curr_edge = 0;

	for(boost::tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
    {
    	edges_G[edge_id++] = *ei; //stores edges of G in the array
    }

	Graph dual_G(num_nodes_dual_G);

	Edge_Index_Array map_g_dual = get(edge_index, G); //maps edges from G to dual
	Edge_Index_Array map_dual_g = get(edge_index, dual_G); //maps edges from dual to G

	edge_weights = get(edge_weight, dual_G);

	for(int i=0; i < num_edges_G; i++){
		file_dual_in.read_edge(src, dest, weight);
		edge = add_edge(src, dest, dual_G).first;
        edge_weights[edge] = weight;
	}

	edge_id = 0;
	curr_edge = 0;

	FILE *fp = file_dual_in.get_file();

	for(boost::tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
    {
    	edges_dual_G[curr_edge] = *ei;  //First store dual_G edges
    	fscanf(fp,"%d", &edge_id);      //read the mapping of dual_to_G edges
    	map_dual_g[*ei] = edge_id;		//map the dual to the prev read value
    	map_g_dual[edges_G[edge_id]] = curr_edge;  //reverse map the edges[G] to curr_edge value
    	curr_edge++;
    }

	file_dual_in.fileClose();

	boost_sp_tree<Graph,Vertex> (0, G);

	return 0;
}