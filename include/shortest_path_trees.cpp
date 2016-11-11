#include "shortest_path_trees.h"

void shortest_path_tree::calculate_sp_tree()
{
#ifdef PRINT
	printf("SP root = %d, ", root_node + 1);
#endif	

	std::vector<bool> in_tree(num_nodes);

	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
			compare_pair> pq;

	distance[root_node] = 0;
	parent[root_node] = -1;
	edge_offsets[root_node] = -1;
	minimum_node_in_path[root_node] = root_node;
	S_value[root_node] = -1;
	S_value_edge[root_node] = -1;

	int src = root_node;

	pq.push(std::make_pair(src, 0));

	while (!pq.empty()) {
		std::pair<int, int> val = pq.top();
		pq.pop();

		if (in_tree[val.first])
			continue;

		in_tree[val.first] = true;

		if(val.first != root_node)
		{
			if(parent[val.first] == root_node)
			{
				S_value[val.first] = val.first;
				S_value_edge[val.first] = edge_offsets[val.first];
			}
			else
			{
				S_value[val.first] = S_value[parent[val.first]];
				S_value_edge[val.first] = S_value_edge[parent[val.first]];
			}

			minimum_node_in_path[val.first] = std::min(val.first, minimum_node_in_path[parent[val.first]]);
		}

		#ifdef PRINT
			printf("%d - %d, ", parent[val.first] + 1, val.first + 1);
		#endif	

		for (unsigned offset = parent_graph.rowOffsets->at(val.first);
				offset < parent_graph.rowOffsets->at(val.first + 1); offset++) {
			int dest = parent_graph.columns->at(offset);

		//do the following
		//1) assign parent
		//2) assign edge_offsets
		//3) assign S_values and S_value_edges
		//4) assign min_node_in_path

			if (!in_tree[dest]) {
				int edge_weight = parent_graph.weights->at(offset);

				if (distance[dest] == -1) {
					distance[dest] = distance[val.first] + edge_weight;
					pq.push(std::make_pair(dest, distance[dest]));
					parent[dest] = val.first;
					edge_offsets[dest] = offset;

				} else if (distance[val.first] + edge_weight < distance[dest]) {
					distance[dest] = distance[val.first] + edge_weight;
					pq.push(std::make_pair(dest, distance[dest]));
					parent[dest] = val.first;
					edge_offsets[dest] = offset;
				}
			}
		}
	}

#ifdef PRINT
	printf("\n");
#endif

	in_tree.clear();

}