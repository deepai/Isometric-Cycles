#ifndef __ISOMETRIC_CYCLE
#define __ISOMETRIC_CYCLE

#include "cycles.h"

void find_isometric_cycles(UF &isometric_cycles, std::vector<std::vector<cycle*> > &sp_cycles,	std::vector<shortest_path_tree*> &sp_trees,
						 	int num_nodes, csr_multi_graph &graph)
{
	for(int x = 0; x < num_nodes; x++)
	{
		for(int j = 0; j < sp_cycles[x].size(); j++)
		{
			cycle curr_cycle = *sp_cycles[x][j];

			int u = graph.rows->at(curr_cycle.edge_offset);
			int v = graph.columns->at(curr_cycle.edge_offset);

			int reverse_edge_offset = graph.reverse_edge->at(curr_cycle.edge_offset); //reverse edge

			cycle *reverse_cycle = cycle::binary_search(sp_cycles[x], 0 , sp_cycles[x].size(), reverse_edge_offset);
			if(reverse_cycle != NULL)
			{
				isometric_cycles.merge(curr_cycle.cycle_index, reverse_cycle->cycle_index);
			}

			//case 1
			if(x == u)
			{
				cycle* c = cycle::binary_search(sp_cycles[v], 0, sp_cycles[v].size(), curr_cycle.edge_offset);
				if(c != NULL)
				{
					isometric_cycles.merge(curr_cycle.cycle_index, c->cycle_index);
				}
			}
			else
			{
				int x_dash = sp_trees[x]->S_value[u];// x' = Sx(u)
				if(x == sp_trees[x_dash]->S_value[v]) //if x == Sx'(v) //case 2
				{
					cycle* c = cycle::binary_search(sp_cycles[x_dash], 0, sp_cycles[x_dash].size() - 1, curr_cycle.edge_offset);
					if(c != NULL)
					{
						isometric_cycles.merge(curr_cycle.cycle_index, c->cycle_index);
					}
				}
				else if(u == sp_trees[v]->S_value[x_dash]) //case 3
				{
					int edge_offset_x_xdash = sp_trees[x]->S_value_edge[x_dash];
					cycle* c = cycle::binary_search(sp_cycles[v], 0, sp_cycles[v].size(), edge_offset_x_xdash);
					if(c != NULL)
					{
						isometric_cycles.merge(curr_cycle.cycle_index, c->cycle_index);
					}
				}
			}
		}
	}
}


int final_count_cycles(UF &isometric_cycles, std::vector<std::vector<cycle*> > &sp_cycles,	std::vector<shortest_path_tree*> &sp_trees,
						 	int num_nodes, csr_multi_graph &graph)
{
	int count = 0;
	int counter = 0;
	for(int x = 0; x < num_nodes; x++)
	{
		for(int j = 0; j < sp_cycles[x].size(); j++)
		{
			int id = sp_cycles[x][j]->cycle_index;

			#ifdef PRINT_CYCLES
				//sp_cycles[x][j]->print_cycle(++counter, graph, *sp_trees[x]);
			#endif

			if(isometric_cycles.parent[id] == id)
			{
				#ifdef PRINT_CYCLES
					sp_cycles[x][j]->print_cycle(count, graph, *sp_trees[x]);
				#endif

				count++;
			}
		}
		//printf("%d\n",count);
	}

	return count;
}

#endif