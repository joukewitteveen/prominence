#include <istream>


enum FeatureType {
    Valley = -1,
    Unknown,
    Peak
};


struct Feature {
    double position;
    double height;
};


/* Turn a one dimensional heightmap on an input stream into a stream of peaks
 * and valleys (features). */
class FeatureReader {
    std::istream& heightmap;
    bool sustain;
    double position;
    double elevation;
    FeatureType last;
  public:
    FeatureReader(std::istream&);
    operator bool () const;
    FeatureReader& operator>> (Feature&);
    FeatureType last_type() const;
};
