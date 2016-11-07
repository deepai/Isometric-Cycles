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


	return 0;
}