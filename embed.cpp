//=======================================================================
// Copyright 2007 Aaron Windsor - Ben Sisson
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>

#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/make_maximal_planar.hpp>
#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/planar_canonical_ordering.hpp>
#include <boost/graph/chrobak_payne_drawing.hpp>
#include <boost/graph/is_straight_line_drawing.hpp>


using namespace boost;


//a class to hold the coordinates of the straight line embedding
struct coord_t{
  std::size_t x;
  std::size_t y;
};

int main(int argc, char** argv){

  typedef adjacency_list
    < vecS,
      vecS,
      undirectedS,
      property<vertex_index_t, int>,
      property<edge_index_t, int>
    > 
    graph;

  graph g(7);
  add_edge(0,1,g);
  add_edge(1,2,g);
  add_edge(2,3,g);
  add_edge(3,0,g);
  add_edge(0,4,g);
  add_edge(1,5,g);
  add_edge(2,6,g);
  add_edge(3,7,g);
  add_edge(4,5,g);
  add_edge(5,6,g);
  add_edge(6,7,g);
  add_edge(7,4,g);

  //Initialize the interior edge index
  property_map<graph, edge_index_t>::type e_index = get(edge_index, g);
  graph_traits<graph>::edges_size_type edge_count = 0;
  graph_traits<graph>::edge_iterator ei, ei_end;
  for(boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    put(e_index, *ei, edge_count++);
  
  
  //Test for planarity; compute the planar embedding as a side-effect
  typedef std::vector< graph_traits<graph>::edge_descriptor > vec_t;
  std::vector<vec_t> embedding(num_vertices(g));
  boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                   boyer_myrvold_params::embedding = 
                                       &embedding[0]
                                   );
  
  make_biconnected_planar(g, &embedding[0]);

  // Re-initialize the edge index, since we just added a few edges
  edge_count = 0;
  for(boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    put(e_index, *ei, edge_count++);


  //Test for planarity again; compute the planar embedding as a side-effect
  boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                   boyer_myrvold_params::embedding = 
                                       &embedding[0]
                                   );

  make_maximal_planar(g, &embedding[0]);

  // Re-initialize the edge index, since we just added a few edges
  edge_count = 0;
  for(boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    put(e_index, *ei, edge_count++);

  boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                   boyer_myrvold_params::embedding = 
                                       &embedding[0]
                                   );

  // Find a canonical ordering
  std::vector<graph_traits<graph>::vertex_descriptor> ordering;
  planar_canonical_ordering(g, &embedding[0], std::back_inserter(ordering));


  //Set up a property map to hold the mapping from vertices to coord_t's
  typedef std::vector< coord_t > straight_line_drawing_storage_t;
  typedef boost::iterator_property_map
    < straight_line_drawing_storage_t::iterator, 
      property_map<graph, vertex_index_t>::type 
    >
    straight_line_drawing_t;

  straight_line_drawing_storage_t straight_line_drawing_storage
    (num_vertices(g));
  straight_line_drawing_t straight_line_drawing
    (straight_line_drawing_storage.begin(), 
     get(vertex_index,g)
     );



  // Compute the straight line drawing
  chrobak_payne_straight_line_drawing(g, 
                                      embedding, 
                                      ordering.begin(),
                                      ordering.end(),
                                      straight_line_drawing
                                      );
  


  std::cout << "The straight line drawing is: " << std::endl;
  graph_traits<graph>::vertex_iterator vi, vi_end;
  for(boost::tie(vi,vi_end) = vertices(g); vi != vi_end; ++vi)
    {
      coord_t coord(get(straight_line_drawing,*vi));
      std::cout << *vi << " -> (" << coord.x << ", " << coord.y << ")" 
                << std::endl;
    }

  // Verify that the drawing is actually a plane drawing
  if (is_straight_line_drawing(g, straight_line_drawing))
    std::cout << "Is a plane drawing." << std::endl;
  else
    std::cout << "Is not a plane drawing." << std::endl;

  return 0;
}