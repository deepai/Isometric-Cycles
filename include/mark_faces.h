#ifndef _H_MARK_INTERNAL_FACES
#define _H_MARK_INTERNAL_FACES

#include <vector>
#include <queue>
#include <forward_list>

#include "common.h"
#include "atomic_linked_list.h"

template<typename T>
int mark_internal_faces(Graph &dual_graph,
	 Edge_Index_Array &edges_G,
	 Vertex external_face,
	 Vertex U,
	 Vertex V,
	 vector<bool> &visited,
	 vector<bool> &is_tree_edge,
	 vector<list_elements<T> > &MCB_TABLE,
	 int cycle_index)
{
	vector<int> list_vertices;
	fill(visited.begin(), visited.end(), 0);

	int degree_count = 0;

	typename graph_traits<Graph>::out_edge_iterator ei, ei_end;

	std::queue<int> bfs_queue;

	bfs_queue.push(U);

	int src, dest;

	visited[U] = true;

	bool connects_external_face = false;

	while(!bfs_queue.empty() && !connects_external_face)
	{
		src = bfs_queue.front();

		list_vertices.push_back(src);

		if(src == external_face)
		{
			while(!bfs_queue.empty())
			{
				bfs_queue.pop();
			}
			connects_external_face = true;
			list_vertices.clear();
			continue;
		}

		bfs_queue.pop();

		boost::tie(ei, ei_end) = out_edges(src, dual_graph);
		
		for(; ei != ei_end; ++ei)
		{
			if(is_tree_edge[edges_G[*ei]])
				continue;

			dest = target(*ei, dual_graph);
			
			if(!visited[dest])
			{
				bfs_queue.push(dest);
				visited[dest] = true;
			}
		}
	}

	if(connects_external_face) //the second tree must be the internal face
	{
		//do the second loop
		visited[V] = true;
		bfs_queue.push(V);

		while(!bfs_queue.empty())
		{
			src = bfs_queue.front();
			MCB_TABLE[src].push(cycle_index);

			degree_count++;

			#ifdef PRINT_CYCLES
				cout << src + 1 << " ";
			#endif

			bfs_queue.pop();

			boost::tie(ei, ei_end) = out_edges(src, dual_graph);

			for(; ei != ei_end; ++ei)
			{
				if(is_tree_edge[edges_G[*ei]])
					continue;

				dest = target(*ei, dual_graph);
				
				if(!visited[dest])
				{
					bfs_queue.push(dest);
					visited[dest] = true;
				}
			}
		}
		#ifdef PRINT_CYCLES
			cout << endl;
		#endif
	}
	else // the first tree is the internal face
	{
		degree_count += list_vertices.size();
		for(auto it = list_vertices.begin(); it != list_vertices.end(); it++)
		{
			MCB_TABLE[*it].push(cycle_index);
			#ifdef PRINT_CYCLES
				cout << *it + 1 << " ";
			#endif
		}
		#ifdef PRINT_CYCLES
			cout << endl;
		#endif
	}

	list_vertices.clear();

	return degree_count;
}


#endif