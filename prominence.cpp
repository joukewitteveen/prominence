#include <iostream>
#include <set>
#include "greader.hpp"

using namespace std;


struct Prominence {
    double prominence;
    Location location;
    bool operator< (Prominence const& other) const {
        // prominences should be sorted in descending order
        return prominence > other.prominence ||
               (prominence == other.prominence && location < other.location);
    }
};


void usage() {
    cout << "Usage: prominence [threshold] <file>" << endl << endl
         << "threshold defaults to " << sea_level << "." << endl << endl
         << "Supported file formats:" << endl
         << "  DOT     (.gv, .dot)" << endl
         << "  GraphML (.graphml, .xml)" << endl
#ifndef NOGDAL
         << "  DEM     (.dem, ...)" << endl
#endif //NOGDAL
         ;
}


// Find the component representative and apply path halving
Location find_root(Location location, Map& heightmap) {
    for (Location* parent = &heightmap[location]._parent;
         *parent != (location = heightmap[*parent]._parent);
         parent = &heightmap[location]._parent)
        *parent = location;

    return location;
}


int main(int argc, char* argv[]) {
    double prominence, threshold = sea_level;
    Map heightmap;
    set<Prominence> peaks;

    switch (--argc) {
      case 2:
        threshold = stod(argv[1]);
      case 1:
        if (read_map(argv[argc], heightmap)) break;
      default:
        usage();
        return 1;
    }

    // Sort the locations in order of decreasing height
    if (num_vertices(heightmap) == 0) return 0;
    HeightOrder by_height(heightmap);
    set<Location, HeightOrder> locations(by_height);
    // Copy all locations (vertices) into the ordered set
    locations.insert(vertices(heightmap).first, vertices(heightmap).second);

    set<Location, HeightOrder> roots(by_height);
    for (auto location : locations) {
        if (heightmap[location].height - sea_level < threshold) break;
        Location& parent = heightmap[location]._parent;

        // Gather all components above and reachable from the current location
        // Duplicates are removed by the set container
        roots.clear();
        for (auto adjacent : adjacent_range(location, heightmap)) {
            if (by_height(adjacent, location)) // adjacent is higher
                roots.insert(find_root(adjacent, heightmap));
        }

        if (roots.empty()) {
            // A peak
            parent = location;
            continue;
        }

        parent = *roots.begin(); // The highest peak among the components
        for (auto root = ++roots.begin(), root_end = roots.end(); // The rest
             root != root_end; ++root) {
            heightmap[*root]._parent = parent; // Necessary for all components

            prominence = heightmap[*root].height - heightmap[location].height;
            if (prominence >= threshold)
                peaks.insert(Prominence { prominence, *root });
        }
    }

    // Highest (isolated) peaks
    for (auto location : locations) {
        prominence = heightmap[location].height - sea_level;
        if (prominence < threshold) break;
        if (heightmap[location]._parent == location) // A remaining root
            peaks.insert(Prominence { prominence, location });
    }

    for (auto p : peaks)
        cout << "Peak " << heightmap[p.location].id
             << " (height: " << heightmap[p.location].height
             << ") has prominence " << p.prominence << endl;

    return 0;
}
