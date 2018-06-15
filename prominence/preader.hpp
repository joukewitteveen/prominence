#include "heightmap.hpp"
#include <set>


struct Prominence {
    Height prominence;
    Location location;
    bool operator< (Prominence const& other) const {
        // Prominences should be sorted in descending order
        return prominence > other.prominence ||
               (prominence == other.prominence && location < other.location);
    }
};


class ProminenceReader {
    typedef std::set<Location, HeightOrder> Locations;
    HeightMap& map;
    HeightOrder by_height;
    Locations locations, roots;
    Locations::iterator location, locations_end, root, roots_end;
    Height const threshold;
    enum class Stage { prominence, highest, done } stage;
    Location find_root(Location); // Find the component representative
    bool explore_location(); // Find adjacent components
    bool read_prominence(Prominence&); // Peaks with a parent
    bool read_highest(Prominence&); // Highest (isolated) peaks
  public:
    ProminenceReader(HeightMap&, Height = sea_level);
    operator bool () const { return stage != Stage::done; }
    ProminenceReader& operator>> (Prominence&);
};
