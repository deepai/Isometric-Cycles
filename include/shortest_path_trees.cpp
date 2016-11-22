#include "shortest_path_trees.h"

int shortest_path_tree::calculate_sp_tree()
{
#ifdef PRINT
	printf("SP root = %d, ", root_node + 1);
#endif	

	std::vector<bool> in_tree(num_nodes);
	std::vector<int> path_length(num_nodes);

	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
			compare_pair> pq;

	distance[root_node] = 0;
	path_length[root_node] = 0;
	parent[root_node] = -1;
	edge_offsets[root_node] = -1;
	minimum_node_in_path[root_node] = root_node;
	S_value[root_node] = -1;
	S_value_edge[root_node] = -1;

	int src = root_node;

	pq.push(std::make_pair(src, 0));

	int count_case_allequal = 0;

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
					path_length[dest] = path_length[val.first] + 1;

				} else if (distance[val.first] + edge_weight < distance[dest]) {
					distance[dest] = distance[val.first] + edge_weight;
					pq.push(std::make_pair(dest, distance[dest]));
					parent[dest] = val.first;
					edge_offsets[dest] = offset;
					path_length[dest] = path_length[val.first] + 1;
				}
				else if (distance[val.first] + edge_weight == distance[dest])
				{
					if(path_length[val.first] + 1 < path_length[dest])
					{
						parent[dest] = val.first;
						edge_offsets[dest] = offset;
						path_length[dest] = path_length[val.first] + 1;
					}
					else if(path_length[val.first] + 1 == path_length[dest])
					{
						int min_existing = parent[dest];
						int min_new = val.first;

						calculate_lca(parent[dest], val.first, min_existing, min_new);
						
						if(min_new < min_existing)
						{
							parent[dest] = val.first;
							edge_offsets[dest] = offset;
							path_length[dest] = path_length[val.first] + 1;
						}

						count_case_allequal += 1;
					}
				}
			}
		}
	}

#ifdef PRINT
	printf("\n");
#endif

	in_tree.clear();
	path_length.clear();

	return count_case_allequal;

}

bool shortest_path_tree::is_non_tree_edge(int row, int col)
{
	return ((parent[col] != row) && (parent[row] != col));
}

void shortest_path_tree::calculate_lca(int existing_parent, int new_parent, int &min_existing, int &min_new)
{
	while(existing_parent != new_parent)
	{
		min_existing = std::min(min_existing, existing_parent);
		min_new = std::min(new_parent, min_new);

		existing_parent = parent[existing_parent];
		new_parent = parent[new_parent];
	}
}