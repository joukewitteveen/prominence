#pragma once


typedef double Height;


constexpr Height sea_level = 0.0;


struct FeatureBase {
    Height height = sea_level;
};


/* Backends provide
 *   Types
 *   - HeightMap
 *   - HeightMapSize
 *   - Location
 *
 *   Element access
 *   - HeightMap[Location].height
 *   - HeightMap[Location].parent
 *
 *   Functions
 *   - num_locations(HeightMap)
 *   - locations(HeightMap)
 *   - adjacent_locations(Location, HeightMap)
 *   - get_name(Location, HeightMap)
 */
#ifdef BACKEND_boost
#include "backend/boost.hpp"
#endif
#ifdef BACKEND_gdal
#include "backend/gdal.hpp"
#endif


class HeightOrder {
    HeightMap& map;
  public:
    HeightOrder(HeightMap& map) : map(map) {}
    bool operator() (Location const& a, Location const& b) const {
        auto a_height = map[a].height, b_height = map[b].height;
        return a_height > b_height || (a_height == b_height && a < b);
    }
};


bool read_map(char const*, HeightMap&);
