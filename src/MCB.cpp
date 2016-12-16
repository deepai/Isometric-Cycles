#include <iostream>
// include <boost/graph/adjacency_list.hpp> // for customizable graphs
#include <iostream>

#include <queue>
#include <boost/ref.hpp>
#include <vector>
#include <bitset>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>

#include <omp.h>

#include "FileReader.h"
#include "FileWriter.h"

#include "common.h"
#include "boost_sp_trees.h"
#include "mark_faces.h"

using namespace std;
using namespace boost;

string input_graph;
string input_dual_graph;


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
	Edge_Iterator *edges_dual_G_Iterator = new Edge_Iterator[num_edges_dual_G]; //store edges iterator of dual_G
	Edge_Index_Array edges_G = get(edge_index, G); //Used to get index of edge corresponding to the given edge
	Edge_Index_Array edges_dual_G = get(edge_index, G); //Used to get index of edge corresponding to the given edge

	vector<int> map_g_dual(num_edges_G);

	int edge_id = 0, curr_edge = 0;

	for(boost::tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
    {
    	edges_G_Iterator[edge_id] = ei; //stores edges of G in the array
    	edges_G[*ei] = edge_id++;
    }

	Graph dual_G(num_nodes_dual_G);

	edge_weights = get(edge_weight, dual_G);

	for(int i=0; i < num_edges_G; i++){
		file_dual_in.read_edge(src, dest, weight);
		edge = add_edge(src, dest, dual_G).first;
        edge_weights[edge] = weight;
	}

	edge_id = 0;
	curr_edge = 0;

	FILE *fp = file_dual_in.get_file();

	for(boost::tie(ei, ei_end) = edges(dual_G); ei != ei_end; ++ei)
    {
    	fscanf(fp,"%d", &edge_id);      //read the mapping of dual_to_G edges
    	edges_dual_G[*ei] = edge_id;
    	map_g_dual[edge_id] = curr_edge;
    	edges_dual_G_Iterator[curr_edge] = ei;
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

vector<vector<boost_cycle<Vertex> > > sp_cycles(num_nodes_G);
vector<pair<boost_cycle<Vertex>,int> > list_cycles;
vector<int> cumulative_sizes(num_nodes_G, 0);

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
					cumulative_sizes[i]++;
				}
			}
		}
	}

	//prefix sum to perform cumulative size calculation of the array.

	int prev = 0, curr;
	for(int i=0; i < num_nodes_G; i++)
	{
		curr = cumulative_sizes[i];
		cumulative_sizes[i] = prev;
		prev += curr;
	}

	int total_num_cycles = 0;
	#pragma omp parallel for reduction(+:total_num_cycles)
	for(int i=0 ;i < num_nodes_G; i++)
	{
		total_num_cycles += sp_cycles[i].size();
	}

	vector<vector<bool> > MCB_TABLE(total_num_cycles, vector<bool>(num_nodes_dual_G));

	//STORE THE LIST OF CYCLES IN A SINGLE LIST AND MAINTAIN REVERSE MAPPING
	list_cycles.resize(total_num_cycles);
	vector<int> reverse_cycle_list_mapping(total_num_cycles);

	total_num_cycles = 0;


	for(int i = 0; i < num_nodes_G; i++)
	{
		for(int j=0; j < sp_cycles[i].size(); j++)
		{
			list_cycles[total_num_cycles].first = sp_cycles[i][j];
			list_cycles[total_num_cycles].second = total_num_cycles;
			total_num_cycles++;
		}
	}

	std::sort(list_cycles.begin(), list_cycles.end(),
			  []( const pair<boost_cycle<Vertex>,int>  &a, const pair<boost_cycle<Vertex>,int> &b ) { return a.first < b.first; });

	cout << "Total Number of Cycles = " << total_num_cycles << endl;


	//THE FOLLOWING EXECUTION MAPS THE POST SORTED CYCLES TO ITS ORIGINAL INDEX i.e. ORIGINAL CYCLE INDEX is present in new location
	for(int i = 0; i < total_num_cycles; i++)
	{
		reverse_cycle_list_mapping[list_cycles[i].second] = i;
	}

	//We consider the first node in the dual graph as the external face.
	Vertex external_face = 0;

	int num_threads = omp_get_max_threads();

	vector<vector<bool> > visited_array(num_threads, vector<bool>(num_nodes_dual_G));

	cout << "Number of threads is " << num_threads << endl;

	#pragma omp parallel for schedule(dynamic)
	for(int i=0; i<num_nodes_G; i++)
	{
		int tid = omp_get_thread_num();
		Vertex U, V;

		for(int j=0; j < sp_cycles[i].size(); j++)
		{
			sp_trees[i]->is_tree_edge[sp_cycles[i][j].edge_id] = true;

			U = source(*edges_dual_G_Iterator[map_g_dual[sp_cycles[i][j].edge_id]], dual_G); //get source vertex corresponding to edge of dual_G
			V = target(*edges_dual_G_Iterator[map_g_dual[sp_cycles[i][j].edge_id]], dual_G); //get target vertex corresponding to edge of dual_G

			assert(U < num_nodes_dual_G && V < num_nodes_dual_G);

			mark_internal_faces(dual_G,
	 							edges_dual_G,
	 							external_face,
	 							U,
	 							V,
	 							visited_array[tid],
	 							sp_trees[i]->is_tree_edge,
	 							MCB_TABLE,
	 							reverse_cycle_list_mapping[cumulative_sizes[i] + j]);

			sp_trees[i]->is_tree_edge[sp_cycles[i][j].edge_id] = false;
		}
	}

	#pragma omp parallel for
	for(int i=0; i < num_threads; i++)
		visited_array[i].clear();

	visited_array.clear();


	#pragma omp parallel for
	for(int i=0; i < num_nodes_G; i++)
	{
		sp_cycles[i].clear();
	}
	sp_cycles.clear();

	cumulative_sizes.clear();
	reverse_cycle_list_mapping.clear();

	#ifdef PRINT_CYCLES

		cout << "printing the MCB matrix..." << endl;

		for(int i=0; i < MCB_TABLE.size(); i++)
		{
			for(int j=0; j< MCB_TABLE[i].size(); j++)
			{
				cout << MCB_TABLE[i][j] << " ";
			}
			cout << endl;
		}
	#endif

	//Next Work Here onwards...

	MCB_TABLE.clear();

	#pragma omp parallel for
	for(int i=0; i < num_nodes_G; i++)
		if(sp_trees[i] != NULL)
			delete sp_trees[i];

	list_cycles.clear();
	sp_trees.clear();

	return 0;
}