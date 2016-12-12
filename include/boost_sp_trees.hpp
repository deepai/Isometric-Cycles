#include "boost_sp_trees.h"
#include <boost/graph/dijkstra_shortest_paths.hpp>

template <class Graph,class Vertex>
void boost_sp_tree<Graph,Vertex>::boost_calculate_sp()
{
#ifdef PRINT
	printf("SP root = %d, ", root_node + 1);
#endif

	std::vector<bool> in_tree(num_nodes);
	std::vector<int> path_length(num_nodes);

	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
		compare_pair> pq;
}