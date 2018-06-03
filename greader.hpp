#include <string>
#include <boost/graph/adjacency_list.hpp>


constexpr double sea_level = 0.0;


struct Feature;


typedef boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::undirectedS,
    Feature>
  Map;
typedef Map::vertex_descriptor Location;


struct Feature {
    std::string id;
    double height = sea_level;
    Location _parent;
};


class HeightOrder {
    Map& map;
  public:
    HeightOrder(Map& map) : map(map) {}
    bool operator()(Location const& a, Location const& b) const {
        auto a_height = map[a].height, b_height = map[b].height;
        return a_height > b_height || (a_height == b_height && a < b);
    }
};


inline boost::iterator_range<boost::graph_traits<Map>::adjacency_iterator>
adjacent_range(Location l, Map const& m) {
    return boost::make_iterator_range(adjacent_vertices(l, m));
}


bool read_map(std::string const, Map&);
