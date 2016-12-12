#ifndef _H_MARK_INTERNAL_FACES
#define _H_MARK_INTERNAL_FACES

#include <vector>
#include <queue>
#include <forward_list>


template <typename filtered_graph, typename Vertex>
void mark_internal_faces(filtered_graph &dual_graph, Vertex external_face, Vertex U, Vertex V, vector<bool> &visited)
{
	forward_list<Vertex> list_vertices;
	fill(list_vertices.begin(), list_vertices.end(), 0);

}


#endif