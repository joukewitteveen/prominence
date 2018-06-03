#include "freader.hpp"
#include <stdexcept>
#include <sstream>
#include <string>

using namespace std;


FeatureReader::FeatureReader(istream& heightmap) :
  heightmap(heightmap), sustain(false), last(Unknown) {
    string line;
    if (!getline(heightmap, line))
        throw logic_error("No input");

    if (!(istringstream(line) >> position >> elevation))
        throw logic_error("Malformed input");
}


FeatureReader::operator bool () const {
    return !heightmap.fail() || sustain;
}


FeatureReader& FeatureReader::operator>> (Feature& feature) {
    string line;

    while (feature.position = position,
           feature.height = elevation,
           getline(heightmap, line)) {
        if (!(stringstream(line) >> position >> elevation) ||
            position <= feature.position)
            throw logic_error("Malformed input");

        if (elevation < feature.height && last != Peak) {
            last = Peak;
            return *this;
        } else if (elevation > feature.height && last != Valley) {
            last = Valley;
            return *this;
        }
    }

    // At the end of the input, process the last known elevation
    if (sustain) {
        // Already done
        sustain = false;
    } else if (last) {
        sustain = true;
        last = static_cast<FeatureType>(-last);
    }
    return *this;
}


FeatureType FeatureReader::last_type() const {
    return last;
}
