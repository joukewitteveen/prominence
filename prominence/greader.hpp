#pragma once

#include <string>
#include <boost/graph/adjacency_list.hpp>

using boost::num_vertices;
using boost::vertices;


constexpr double sea_level = 0.0;


struct Feature;


typedef boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::undirectedS,
    Feature>
  HeightMap;
typedef HeightMap::vertex_descriptor Location;


struct Feature {
    std::string id;
    double height = sea_level;
    Location parent;
};


class HeightOrder {
    HeightMap& map;
  public:
    HeightOrder(HeightMap& map) : map(map) {}
    bool operator()(Location const& a, Location const& b) const {
        auto a_height = map[a].height, b_height = map[b].height;
        return a_height > b_height || (a_height == b_height && a < b);
    }
};


inline boost::iterator_range<boost::graph_traits<HeightMap>::adjacency_iterator>
adjacent_range(Location l, HeightMap const& m) {
    return boost::make_iterator_range(boost::adjacent_vertices(l, m));
}


bool read_map(std::string const, HeightMap&);
