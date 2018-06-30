#include "preader.hpp"


/* Each location points to a higher location in its connected component (if one
 * exists). This function finds the highest location of a connected component.
 * It applies path halving to accelerate future invocations. */
Location ProminenceReader::find_root(Location location) {
    for (Location* parent = &map[location].parent;
         *parent != (location = map[*parent].parent);
         parent = &map[location].parent)
        *parent = location; // Path halving

    return location;
}


/* The locations reachable from the current location are examined. If they are
 * higher than the current location, then (a representative of) their connected
 * component is added to a set of roots. The current location connects all
 * components in the set of roots, hence the components are merged. */
bool ProminenceReader::explore_location() {
    if (location == locations_end ||
        map[*location].height - sea_level < threshold) return false;

    Location& parent = map[*location].parent;

    // Gather all components above and reachable from the current location
    // Duplicates are removed by the set container
    roots.clear();
    for (auto adjacent : adjacent_locations(*location, map)) {
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


/* The prominence of all but one (the highest) of the highest peaks of the root
 * components is known when merging the components. When all roots have been
 * examined, move to the next location and repeat. */
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


/* After exploring all locations, we are left with a set of isolated (in the
 * reachability graph underlying the heightmap) peaks. By definition, the
 * prominence of each such peak is its height. */
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


/* Prepare an exploration of all locations on the map in order of decreasing
 * height. */
ProminenceReader::ProminenceReader(HeightMap& map, Height threshold) :
  map(map),
  by_height(map),
  locations(by_height),
  roots(by_height),
  threshold(threshold) {
    // Copy and sort all locations
    auto locations_iterator = ::locations(map);
    locations.insert(locations_iterator.begin(), locations_iterator.end());
    location = locations.begin();
    locations_end = locations.end();
    stage = explore_location() ? Stage::prominence : Stage::done;
}


/* Obtain the next known prominence. */
ProminenceReader& ProminenceReader::operator>> (Prominence& p) {
    if (stage == Stage::prominence && !read_prominence(p)) {
        location = locations.begin();
        stage = Stage::highest;
    }
    if (stage == Stage::highest && !read_highest(p))
        stage = Stage::done;

    return *this;
}
