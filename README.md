# chrobak_embedding
Planar embedding using Boost C++. Segmentation Fault free. 

This is a combination of the various Boost C++ examples invoving make_connected, make_biconnected_planar, make_maximal_planar and chrobak_payne_straight_line_drawing. Nearly all of the credit is due to Aaron Windsor and Boost (http://www.boost.org/), I just got a working example that required the combination of the various helper functions. Hopefully, this should provide a clear guide to anyone who wants to embed planar graphs using this software.

Compile using 
  g++ embed.cpp
Run using 
  ./a.out
