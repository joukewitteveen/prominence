#include <string>
#include <boost/graph/adjacency_list.hpp>


inline std::vector<std::vector<char const*>> supported_formats() {
    return {{"DOT",     "(.gv, .dot)"},
            {"GraphML", "(.graphml, .xml)"}};
}


struct Feature;

typedef boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::undirectedS,
    Feature>
  HeightMap;

typedef HeightMap::vertex_descriptor Location;

typedef HeightMap::vertices_size_type HeightMapSize;


struct Feature : FeatureBase {
    Location parent;
    std::string id;
};


inline HeightMapSize num_locations(HeightMap const& m) {
    return boost::num_vertices(m);
}

inline boost::iterator_range<boost::graph_traits<HeightMap>::vertex_iterator>
locations(HeightMap const& m) {
    return boost::make_iterator_range(boost::vertices(m));
}

inline boost::iterator_range<boost::graph_traits<HeightMap>::adjacency_iterator>
adjacent_locations(Location l, HeightMap const& m) {
    return boost::make_iterator_range(boost::adjacent_vertices(l, m));
}

inline std::string get_name(Location l, HeightMap const& m) {
    return m[l].id;
}
