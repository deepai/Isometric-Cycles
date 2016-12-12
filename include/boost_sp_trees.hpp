#include "boost_sp_trees.h"
#include <boost/graph/dijkstra_shortest_paths.hpp>

template <class Graph,class Vertex,class Edge_Weight_Array,class Edge_Index_Array>
int boost_sp_tree<Graph,Vertex,Edge_Weight_Array,Edge_Index_Array>::boost_calculate_sp()
{
#ifdef PRINT
	printf("SP root = %d, ", root_node + 1);
#endif

	std::vector<bool> in_tree(num_nodes);
	std::vector<int> path_length(num_nodes);
	std::vector<int> edge_offsets(count_edges);

	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
		compare_pair> pq;

	D[root] = 0;
	path_length[root] = 0;
	Parent[root] = -1;
	Min_node[root] = root;
	S[root] = -1;
	edge_offsets[root] = -1;

	Vertex src = root;

	pq.push(make_pair(src,0));

	int count_case_allequal = 0;

	typename boost::graph_traits<Graph>::out_edge_iterator adj_begin,adj_end;
	typename boost::graph_traits<Graph>::edge_descriptor edge;

	int edge_weight, dest, edge_index;

	while(!pq.empty())
	{
		pair<int, int> val = pq.top();
		pq.pop();

		if (in_tree[val.first])
			continue;

		in_tree[val.first] = true;

		if(val.first != root)
		{
			if(Parent[val.first] == root)
			{
				S[val.first] = val.first;
			}
			else
			{
				S[val.first] = S[Parent[val.first]];
			}

			Min_node[val.first] = std::min(val.first, Min_node[Parent[val.first]]);
			is_tree_edge[edge_offsets[val.first]] = true;
		}

		#ifdef PRINT
			printf("%d - %d, ", Parent[val.first] + 1, val.first + 1);
		#endif

		tie(adj_begin,adj_end) = out_edges(val.first, input_graph);

		for(; adj_begin != adj_end; ++adj_begin)
		{
			edge = *adj_begin;
			edge_weight = edge_weights[edge];
			edge_index = edge_indexes[edge];

			dest = target(edge, input_graph);

			if(D[val.first] + edge_weight < D[dest])
			{
				D[dest] = D[val.first] + edge_weight;
				pq.push({dest, D[dest]});
				Parent[dest] = val.first;
				path_length[dest] = path_length[val.first] + 1;
				edge_offsets[dest] = edge_index;
			}
			else if(D[val.first] + edge_weight < D[dest])
			{
				if(path_length[val.first] + 1 < path_length[dest])
				{
					Parent[dest] = val.first;
					edge_offsets[dest] = edge_index;
					path_length[dest] = path_length[val.first] + 1;
				}
				else if(path_length[val.first] + 1 == path_length[dest])
				{
					int min_existing = Parent[dest];
					int min_new = val.first;

					calculate_lca(Parent[dest], val.first, min_existing, min_new);
					
					if(min_new < min_existing)
					{
						Parent[dest] = val.first;
						edge_offsets[dest] = edge_index;
						path_length[dest] = path_length[val.first] + 1;
					}

					count_case_allequal += 1;
				}
			}
		}
	}

	in_tree.clear();
	path_length.clear();
	edge_offsets.clear();

	return count_case_allequal;
}

template <class Graph,class Vertex,class Edge_Weight_Array, class Edge_Index_Array>
void boost_sp_tree<Graph,Vertex,Edge_Weight_Array,Edge_Index_Array>::calculate_lca(int existing_parent, int new_parent, int &min_existing, int &min_new)
{
	while(existing_parent != new_parent)
	{
		min_existing = std::min(min_existing, existing_parent);
		min_new = std::min(new_parent, min_new);

		existing_parent = Parent[existing_parent];
		new_parent = Parent[new_parent];
	}
}