#include <iostream>
// include <boost/graph/adjacency_list.hpp> // for customizable graphs
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>

#include <cstdlib>
#include <ctime>

#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/make_connected.hpp>
#include <boost/graph/make_biconnected_planar.hpp>

#include "FileReader.h"
#include "FileWriter.h"

using namespace std;
using namespace boost;

string input_graph;
string input_dual_graph;

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
typedef property_map<Graph, edge_weight_t>::type Edge_Weight_Array;
typedef property_map<Graph, edge_index_t>::type Edge_Index_Array;
typedef graph_traits<Graph>::edge_iterator Edge_Iterator;

int main(int argc, char *argv[])
{

	return 0;
}