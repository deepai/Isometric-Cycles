#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <set>
#include <omp.h>
#include <string>
#include <algorithm>
#include <atomic>
#include <list>
#include <cstring>
#include <unordered_set>
#include <unordered_map>
#include <utility>

#include "FileReader.h"
#include "Files.h"
#include "utils.h"
#include "Host_Timer.h"
#include "CsrGraph.h"
#include "CsrTree.h"
#include "CsrGraphMulti.h"
#include "bit_vector.h"
#include "stats.h"
#include "shortest_path_trees.h"
#include "cycles.h"
#include "UF.h"
#include "check_isometry.h"

HostTimer globalTimer;

std::string InputFileName;
std::string OutputFileDirectory;

int num_threads;

int main(int argc, char **argv)
{
	if (argc < 4) {
		printf("Ist Argument should indicate the InputFile\n");
		printf("2nd Argument should indicate the OutputFile\n");
		printf("3th argument should indicate the number of threads.(Optional) (1 default)\n");
		exit(1);
	}

	if (argc >= 4)
		num_threads = atoi(argv[3]);

	InputFileName = argv[1];

	omp_set_num_threads(num_threads);

	//Open the FileReader class
	std::string InputFilePath = InputFileName;

	//Read the Inputfile.
	FileReader Reader(InputFilePath.c_str());

	int v1, v2, Initial_Vertices, weight;

	int nodes, edges;

	//firt line of the input file contains the number of nodes and edges
	Reader.get_nodes_edges(nodes, edges);

	csr_multi_graph *graph = new csr_multi_graph();

	graph->Nodes = nodes;
	graph->initial_edge_count = edges;
	/*
	 * ====================================================================================
	 * Fill Edges.
	 * ====================================================================================
	 */
	for (int i = 0; i < edges; i++) {
		Reader.read_edge(v1, v2, weight);
		graph->insert(v1, v2, weight, false);
	}

	graph->calculateDegreeandRowOffset();
	Reader.fileClose();

	int source_vertex = 0;

	
	csr_tree *initial_spanning_tree = new csr_tree(graph);
	initial_spanning_tree->populate_tree_edges(true, source_vertex);


	std::vector<int> non_tree_edges_map(graph->rows->size());
	std::fill(non_tree_edges_map.begin(), non_tree_edges_map.end(), -1);

	for (int i = 0; i < initial_spanning_tree->non_tree_edges->size(); i++)
		non_tree_edges_map[initial_spanning_tree->non_tree_edges->at(i)] = i;

	for (int i = 0; i < graph->rows->size(); i++) {
		//copy the edges into the reverse edges as well.
		if (non_tree_edges_map[i] < 0)
			if (non_tree_edges_map[graph->reverse_edge->at(i)] >= 0)
				non_tree_edges_map[i] =
						non_tree_edges_map[graph->reverse_edge->at(i)];
	}

#ifdef PRINT_CYCLES
	for(int i = 0; i < graph->rows->size(); i++) {
		if(non_tree_edges_map[i] >=0 )
		{
			int row = graph->rows->at(i);
			int col = graph->columns->at(i);

			printf("Non-Tree Edge: %d - %d\n", row + 1, col + 1);
		}
	}
#endif

	std::vector<shortest_path_tree*> sp_trees(nodes);
	std::vector<std::vector<cycle*> > sp_cycles(nodes);

int count_case_all_equal = 0;
//calculate and construct shortest path trees.
#ifndef PRINT_CYCLES
#pragma omp parallel for reduction(+:count_case_all_equal)
#endif
	for(int i = 0; i < nodes; ++i)
	{
		sp_trees[i] = new shortest_path_tree(nodes,i,*graph);
		count_case_all_equal += sp_trees[i]->calculate_sp_tree();
	}

//check for every non-tree edge, if a valid cycle is formed. if the cycle is valid, add it.
#ifndef PRINT_CYCLES
#pragma omp parallel for
#endif
	for(int i=0; i < nodes; i++)
	{
		for(int j=0; j < non_tree_edges_map.size(); j++)
		{
			int row = graph->rows->at(j); //get the row
			int col = graph->columns->at(j); //get the col

			if(sp_trees[i]->is_non_tree_edge(row,col))
			{
				cycle* c = cycle::get_cycle_info(*sp_trees[i], j);
				if(c != NULL)
				{
					sp_cycles[i].push_back(c);
				}
			}
		}
	}

size_t total_num_cycles = 0;

//reduce and sort each cycle by its edge_offset_id
#pragma omp parallel for reduction(+:total_num_cycles)
	for(int i=0; i < nodes; i++)
	{
		std::sort(sp_cycles[i].begin(), sp_cycles[i].end(), compare_cycle());
		total_num_cycles += sp_cycles[i].size();
	}

printf("The total number of cycles = %d\n", total_num_cycles);

std::vector<int> cycle_offset_nodes(nodes);

int prev = 0, curr;
//exclusive sum.
for(int i = 0; i < nodes; i++)
{
	curr = sp_cycles[i].size();
	cycle_offset_nodes[i] = prev;
	prev += curr;
}

//mark the indices.
#pragma omp parallel for
	for(int i=0; i < nodes ;i++)
	{
		for(int j=0; j<sp_cycles[i].size(); j++)
			sp_cycles[i][j]->set_index(cycle_offset_nodes[i] + j);
	}

UF isometric_cycles(total_num_cycles);

find_isometric_cycles(isometric_cycles, sp_cycles,	sp_trees, nodes, *graph);

int final_isometric_cycle = final_count_cycles(isometric_cycles, sp_cycles,	sp_trees, nodes, *graph);

printf("The total number of isometric cycles = %d\n", final_isometric_cycle);
printf("Total Number of cases for equal paths = %d\n", count_case_all_equal);

	//clear the memory
#pragma omp parallel for
	for(int i=0; i < nodes; i++)
	{
		sp_cycles[i].clear();
	}

#pragma omp parallel for
	for(int i=0; i < nodes; i++)
		if(sp_trees[i] != NULL)
			delete sp_trees[i];

	sp_trees.clear();
	sp_cycles.clear();

	return 0;
}