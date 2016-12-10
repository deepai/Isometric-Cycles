#include "boost_sp_trees.h"
#include <boost/graph/dijkstra_shortest_paths.hpp>

template <class Graph,class Vertex>
void boost_sp_tree<Graph,Vertex>::boost_calculate_sp()
{
	dijkstra_shortest_paths(input_graph, info.root,
          				  predecessor_map(boost::make_iterator_property_map(info.Parent.begin(), get(vertex_index, input_graph))).
                          distance_map(boost::make_iterator_property_map(info.D.begin(), get(vertex_index, input_graph))).visitor(vis));
}