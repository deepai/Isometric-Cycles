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

#include <omp.h>

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
typedef graph_traits<Graph>::out_edge_iterator Out_Edge_Iterator;

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

	Edge_Iterator *edges_G_Iterator = new Edge_Iterator[num_edges_G]; //stores edge iterator of G
	Edge_Iterator *edges_dual_G_Iterator = new Edge_Iterator[num_edges_G]; //stores edge iterator of dual_G
	Edge_Index_Array edges_G = get(edge_index, G); //Used to get index of edge corresponding to the given edge

	int edge_id = 0, curr_edge = 0;

	for(boost::tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
    {
    	edges_G_Iterator[edge_id] = ei; //stores edges of G in the array
    	edges_G[*ei] = edge_id++;
    }

	Graph dual_G(num_nodes_dual_G);

	vector<int> map_g_dual(num_edges_G); //index containing mapping from g to dual edges
	vector<int> map_dual_g(num_edges_G); //index containing mapping from dual to g edges

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
    	edges_dual_G_Iterator[curr_edge] = ei;  //First store dual_G edges
    	fscanf(fp,"%d", &edge_id);      //read the mapping of dual_to_G edges
    	map_dual_g[curr_edge] = edge_id;		//map the dual to the prev read value
    	map_g_dual[edge_id] = curr_edge;  //reverse map the edges[G] to curr_edge value
    	curr_edge++;
    }

	file_dual_in.fileClose();

	std::vector<boost_sp_tree<Graph,Vertex,Edge_Weight_Array,Edge_Index_Array>*> sp_trees(num_nodes_G);

	int count_case_all_equal = 0;
//calculate and construct shortest path trees.
#ifndef PRINT_CYCLES
#pragma omp parallel for reduction(+:count_case_all_equal)
#endif
	for(int i = 0; i < num_nodes_G; ++i)
	{
		sp_trees[i] = new boost_sp_tree<Graph,Vertex,Edge_Weight_Array,Edge_Index_Array>(i, G, edge_weights, edges_G);
		count_case_all_equal += sp_trees[i]->boost_calculate_sp();
	}

cout << count_case_all_equal << endl;

vector<vector<boost_cycle<Vertex,Edge_Iterator> > > sp_cycles(num_nodes_G);
vector<boost_cycle<Vertex, Edge_Iterator> > list_cycles;

#ifndef PRINT_CYCLES
#pragma omp parallel for reduction(+:count_case_all_equal)
#endif
	for(int i = 0; i < num_nodes_G; ++i)
	{
		Vertex U,V;

		for(int j = 0; j < num_edges_G; j++)
		{
			if(!sp_trees[i]->is_tree_edge[j])
			{
				ei = edges_G_Iterator[j];
				U = source(*ei, G);
				V = target(*ei, G);

				if(sp_trees[i]->is_cycle(U, V))
				{
					sp_cycles[i].push_back(sp_trees[i]->get_cycle(ei, U, V));
				}
			}
		}
	}

	int total_num_cycles = 0;
	#pragma omp parallel for reduction(+:total_num_cycles)
	for(int i=0 ;i < num_nodes_G; i++)
	{
		total_num_cycles += sp_cycles[i].size();
	}

	list_cycles.resize(total_num_cycles);
	total_num_cycles = 0;

	for(int i = 0;i < num_nodes_G; i++)
	{
		for(int j=0; j < sp_cycles[i].size(); j++)
		{
			list_cycles[total_num_cycles++] = sp_cycles[i][j]; 
		}
	}

	cout << "Total Number of Cycles = " << total_num_cycles << endl;


	#pragma omp parallel for
	for(int i=0; i < num_nodes_G; i++)
	{
		sp_cycles[i].clear();
	}
	sp_cycles.clear();


	std::sort(list_cycles.begin(), list_cycles.end());


	#pragma omp parallel for
	for(int i=0; i < num_nodes_G; i++)
		if(sp_trees[i] != NULL)
			delete sp_trees[i];

	sp_trees.clear();

	return 0;
}