#ifndef __H_INCLUDE_COMMON
#define __H_INCLUDE_COMMON

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>


#include <vector>

using namespace boost;
using namespace std;

typedef adjacency_list
< vecS,
  vecS,
  undirectedS,
  property<vertex_index_t, int>,
  property<edge_index_t, int,
  property<edge_weight_t,int> >
> 
Graph;

typedef graph_traits<Graph>::edge_descriptor Edge;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef property_map<Graph, edge_weight_t>::type Edge_Weight_Array;
typedef property_map<Graph, edge_index_t>::type Edge_Index_Array;
typedef graph_traits<Graph>::edge_iterator Edge_Iterator;
typedef graph_traits<Graph>::out_edge_iterator Out_Edge_Iterator;



#endif