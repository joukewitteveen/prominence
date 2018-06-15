#include <cstddef>
#include <string>
#include <vector>


inline std::vector<std::vector<char const*>> supported_formats() {
    return {{"DEM", "(.dem, ...)"}};
}


typedef std::size_t Location;


struct Feature : FeatureBase {
    Location parent;
};


typedef std::vector<Feature>::size_type HeightMapSize;


class HeightMap {
    friend HeightMapSize num_locations(HeightMap const&);
    friend std::vector<Location> adjacent_locations(Location, HeightMap const&);
    friend std::string get_name(Location, HeightMap const&);
    friend bool read_map(char const*, HeightMap&);
    HeightMap(HeightMapSize x, HeightMapSize y) : xsize(x), map(x * y) {}
    HeightMapSize xsize;
    double geo_transform[6];
    std::vector<Feature> map;
  public:
    HeightMap() : xsize(0) {}
    Feature& operator[] (Location l) { return map[l]; }
    Feature const& operator[] (Location l) const { return map[l]; }
};


class LocationIterator {
    class iterator {
        Location location;
      public:
        iterator(Location l) : location(l) {}
        iterator& operator++ () { ++location; return *this; }
        bool operator!= (iterator const& other) const {
            return location != *other;
        }
        Location operator* () const { return location; }
    };
    Location const size;
  public:
    LocationIterator(Location size) : size(size) {}
    iterator begin() const { return iterator(0); }
    iterator end() const { return iterator(size); }
};


inline HeightMapSize num_locations(HeightMap const& m) {
    return m.map.size();
}

inline LocationIterator locations(HeightMap const& m) {
    return LocationIterator(num_locations(m));
}

std::vector<Location> adjacent_locations(Location, HeightMap const&);

std::string get_name(Location, HeightMap const&);
