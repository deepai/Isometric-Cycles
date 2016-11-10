#include "shortest_path_trees.h"

void shortest_path_tree::calculate_sp_tree()
{
	std::vector<bool> in_tree(num_nodes);

	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
			compare_pair> pq;

	distance[root_node] = 0;
	parent[root_node] = -1;
	edge_offsets[root_node] = -1;

	int src = root_node;

	pq.push(std::make_pair(src, 0));

	while (!pq.empty()) {
		std::pair<int, int> val = pq.top();
		pq.pop();

		if (in_tree[val.first])
			continue;

		in_tree[val.first] = true;

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

					S_value[dest] = (val.first == root_node)?root_node:S_value[val.first];
					S_value_edge[dest] = (val.first == root_node)?offset : S_value_edge[val.first];

					minimum_node_in_path[dest] = std::min(dest,val.first);

				} else if (distance[val.first] + edge_weight < distance[dest]) {
					distance[dest] = distance[val.first] + edge_weight;
					pq.push(std::make_pair(dest, distance[dest]));
					parent[dest] = val.first;
					edge_offsets[dest] = offset;

					S_value[dest] = (val.first == root_node)?root_node:S_value[val.first];
					S_value_edge[dest] = (val.first == root_node)?offset : S_value_edge[val.first];

					minimum_node_in_path[dest] = std::min(dest,val.first);
				}
			}
		}
	}

	in_tree.clear();

}