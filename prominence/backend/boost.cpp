#include "../heightmap.hpp"
#include <fstream>
#include <boost/graph/graphml.hpp>
#include <boost/graph/graphviz.hpp>

using namespace std;
using namespace boost;


bool read_map(char const* uri, HeightMap& map) {
    string const filename(uri);
    //TODO: This is a hack assuming string::npos == -1
    string const ext = filename.substr(filename.find_last_of('.') + 1);
    ifstream in(uri);
    if (!in) return false;

    dynamic_properties dp(ignore_other_properties);
    dp.property("node_id", get(&Feature::id, map));
    dp.property("height", get(&Feature::height, map));

    if (ext == "graphml" || ext == "xml") {
        read_graphml(in, map, dp);
        return true;
    } else if (ext == "gv" || ext == "dot")
        return read_graphviz(in, map, dp);
    return false;
}
