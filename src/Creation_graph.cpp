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

#include <create_dual_graph.hpp>
#include <Graph_To_File.hpp>

using namespace std;

using namespace boost;

std::string input_file, output_file, dual_output_file;

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
    srand(time(NULL));

	if(argc < 2)
	{
		printf("Ist argument is the name of the input file.\n");
		return 1;
	}
    if(argc == 3)
    {
        output_file = string(argv[2]);
    }

    if(argc == 4)
    {
        output_file = string(argv[2]);
        dual_output_file = string(argv[3]);
    }


	input_file = string(argv[1]);

	int num_nodes_count, num_edges_count;
	int src, dest, weight;
	FileReader file_in(input_file.c_str());

	file_in.get_nodes_edges(num_nodes_count, num_edges_count);

	Graph G(num_nodes_count);
    Edge edge; 

    Edge_Weight_Array edge_weights = get(edge_weight, G);

	for(int i=0; i < num_edges_count; i++){
		file_in.read_edge(src, dest, weight);
		edge = add_edge(src, dest, G).first;
        edge_weights[edge] = weight;
	}

	file_in.fileClose();

    make_connected(G);

	Edge_Index_Array e_index = get(edge_index, G);
    graph_traits<Graph>::edges_size_type edge_count = 0;
    Edge_Iterator ei, ei_end;
    for(boost::tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
    {
        e_index[*ei] = edge_count++;
    }

    embedding_storage_t embedding_storage(num_vertices(G));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index, G));

    bool is_planar_graph = false;
    is_planar_graph = boyer_myrvold_planarity_test(boyer_myrvold_params::graph = G,
                                                    boyer_myrvold_params::embedding = embedding
                                   			);

    // for (tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
    //         cout << edge_weights[*ei] << endl;

    int prev_edges = num_edges(G);

    cout << "Number of original Edges: " << prev_edges << endl;


    if(is_planar_graph)
    {
    	vertex_output_visitor v_vis;
    	edge_output_visitor e_vis;

        make_biconnected_planar(G, embedding);

        edge_count = 0;

        edge_weights = get(edge_weight, G);
        for(boost::tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
        {
                e_index[*ei] = edge_count++;
        }

        is_planar_graph = boyer_myrvold_planarity_test(boyer_myrvold_params::graph = G,
                                                    boyer_myrvold_params::embedding = embedding
                                            );

        if(is_planar_graph)
        {
            int current_edges = num_edges(G);
            cout << "Number of new Edges: " << current_edges << endl;

            //This index is used to map the index value to the individual edge descriptors
            Edge *index_to_edges_map = new Edge[current_edges];

            edge_count = 0;

            for(boost::tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
            {
                    index_to_edges_map[edge_count] = *ei;
                    e_index[*ei] = edge_count++;
            }


            Graph dual_graph;
            create_dual_graph(G, dual_graph, embedding);


            if(argc == 3)
            {
                write_graph_to_file<Graph,Edge_Iterator,Edge_Weight_Array>(output_file.c_str(), G);
            }
            else if(argc == 4)
            {
                write_graph_to_file<Graph,Edge_Iterator,Edge_Weight_Array>(output_file.c_str(), G);
                write_graph_to_file<Graph,Edge_Iterator,Edge_Weight_Array>(dual_output_file.c_str(), dual_graph);
            }

            vertex_output_visitor v_vis;
            planar_face_traversal(G, embedding, v_vis);
            cout << "Number of faces = " << number_faces << endl;

            cout << "Number of vertices in dual graph = " << num_vertices(dual_graph) << endl;

            delete[] index_to_edges_map;

        }
    }

    cout << "Planarity test for graph yields " << ((is_planar_graph)?"true":"false") << endl;
    
    return 0;
}