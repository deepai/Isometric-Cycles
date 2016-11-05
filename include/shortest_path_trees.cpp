#include "shortest_path_trees.h"

void shortest_path_tree::calculate_sp_tree()
{
	std::vector<int> distance(num_nodes,-1);
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
			unsigned column = parent_graph.columns->at(offset);
			if (!in_tree[column]) {
				int edge_weight = parent_graph.weights->at(offset);

				if (distance[column] == -1) {
					distance[column] = distance[val.first] + edge_weight;
					pq.push(std::make_pair(column, distance[column]));
					parent[column] = val.first;
					edge_offsets[column] = offset;

				} else if (distance[val.first] + edge_weight
						< distance[column]) {
					distance[column] = distance[val.first] + edge_weight;
					pq.push(std::make_pair(column, distance[column]));
					parent[column] = val.first;
					edge_offsets[column] = offset;
				}
			}
		}
	}

	distance.clear();
	in_tree.clear();

}