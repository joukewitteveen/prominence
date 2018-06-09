#include "preader.hpp"


Location ProminenceReader::find_root(Location location) {
    for (Location* parent = &map[location].parent;
         *parent != (location = map[*parent].parent);
         parent = &map[location].parent)
        *parent = location; // Path halving

    return location;
}


bool ProminenceReader::explore_location() {
    if (location == locations_end ||
        map[*location].height - sea_level < threshold) return false;

    Location& parent = map[*location].parent;

    // Gather all components above and reachable from the current location
    // Duplicates are removed by the set container
    roots.clear();
    for (auto adjacent : adjacent_range(*location, map)) {
        if (by_height(adjacent, *location)) // adjacent is higher
            roots.insert(find_root(adjacent));
    }
    root = roots.begin();
    roots_end = roots.end();

    // Set the parent to the peak itself, or to the highest root
    parent = roots.empty() ? *location : *root++;
    for (auto r = root; r != roots_end; ++r)
        map[*r].parent = parent;

    return true;
}


bool ProminenceReader::read_prominence(Prominence& p) {
    do {
        if (root != roots_end) {
            p = Prominence {
                    map[*root].height - map[*location].height,
                    *root++
            };
            if (p.prominence >= threshold) return true;
        }
        ++location;
    } while (explore_location());
    return false;
}


bool ProminenceReader::read_highest(Prominence& p) {
    while (location != locations_end) {
        p = Prominence {
            map[*location].height - sea_level,
            *location++
        };
        if (p.prominence < threshold) break;
        if (map[p.location].parent == p.location) return true;
    }
    return false;
}


ProminenceReader::ProminenceReader(HeightMap& map, double threshold) :
  map(map),
  by_height(map),
  locations(by_height),
  roots(by_height),
  threshold(threshold) {
    // Copy and sort all locations (vertices)
    locations.insert(vertices(map).first, vertices(map).second);
    location = locations.begin();
    locations_end = locations.end();
    stage = explore_location() ? Stage::prominence : Stage::done;
}


ProminenceReader& ProminenceReader::operator>> (Prominence& p) {
    if (stage == Stage::prominence && !read_prominence(p)) {
        location = locations.begin();
        stage = Stage::highest;
    }
    if (stage == Stage::highest && !read_highest(p))
        stage = Stage::done;

    return *this;
}
