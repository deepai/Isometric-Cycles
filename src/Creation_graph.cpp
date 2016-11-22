#include <iostream>
// include <boost/graph/adjacency_list.hpp> // for customizable graphs
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>

#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

#include "FileReader.h"

using namespace std;

using namespace boost;

std::string input_file;

typedef adjacency_list
< vecS,
  vecS,
  undirectedS,
  property<vertex_index_t, int>,
  property<edge_index_t, int>
> 
Graph;

int number_faces = 0;

typedef vector<vector<graph_traits<Graph>::edge_descriptor > > embedding_storage_t;
typedef iterator_property_map<embedding_storage_t::iterator, property_map<Graph,vertex_index_t>::type > embedding_t;


// Some planar face traversal visitors that will 
// print the vertices and edges on the faces

// Some planar face traversal visitors that will 
// print the vertices and edges on the faces

struct output_visitor : public planar_face_traversal_visitor
{
  void begin_face()
  {
  	number_faces++;
  }
  void end_face()
  {
  	#ifdef PRINT_FACES
  		printf("\n");		
  	#endif
  }
};



struct vertex_output_visitor : public output_visitor
{
  template <typename Vertex> 
  void next_vertex(Vertex v) 
  {
  	#ifdef PRINT_FACES
  		printf("%d ",v + 1);		
  	#endif
  }
};



struct edge_output_visitor : public output_visitor
{
  template <typename Edge> 
  void next_edge(Edge e) 
  { 

  }
};



int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Ist argument is the name of the input file.\n");
		return 1;
	}
	input_file = string(argv[1]);

	int num_nodes, num_edges;
	int src, dest, weight;
	FileReader file_in(input_file.c_str());

	file_in.get_nodes_edges(num_nodes, num_edges);

	Graph G(num_nodes);

	for(int i=0; i < num_edges; i++){
		file_in.read_edge(src, dest, weight);
		add_edge(src, dest, weight, G);
	}


	file_in.fileClose();

	property_map<Graph, edge_index_t>::type e_index = get(edge_index, G);
  	graph_traits<Graph>::edges_size_type edge_count = 0;
  	graph_traits<Graph>::edge_iterator ei, ei_end;
  	for(boost::tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
    	put(e_index, *ei, edge_count++);

	embedding_storage_t embedding_storage(num_vertices(G));
	embedding_t embedding(embedding_storage.begin(), get(vertex_index, G));

	bool is_planar_graph = false;
	is_planar_graph = boyer_myrvold_planarity_test(boyer_myrvold_params::graph = G,
                               							boyer_myrvold_params::embedding = embedding
                               							);

	if(is_planar_graph)
	{
		vertex_output_visitor v_vis;
		edge_output_visitor e_vis;

		planar_face_traversal(G, embedding, v_vis);

		cout << "Number of faces = " << number_faces << endl;
	}

	cout << "Planarity test for graph yields " << ((is_planar_graph)?"true":"false") << endl;


	return 0;
}