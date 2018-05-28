#include <iostream>
#include <set>
#include <stack>
#include "freader.hpp"

using namespace std;


struct Prominence {
    double prominence;
    Feature peak;
    bool operator< (Prominence const& other) const {
        // Prominences should be sorted in descending order
        return prominence > other.prominence;
    }
};


class ProminenceReader {
    FeatureReader& features;
    stack<Prominence> partial; // Peaks with only a known left-sided prominence
    const double sea_level;
    bool reader_good;
    bool reference_good;
    Prominence reference; // Rightmost peak under investigation
    bool update_reference(); // Advance to the next peak
  public:
    ProminenceReader(FeatureReader& features);
    operator bool () const { return reader_good; }
    ProminenceReader& operator>> (Prominence&);
};


bool ProminenceReader::update_reference() {
    double valley_height;

    partial.push(reference);

    // Read a valley
    if(!(features >> reference.peak)) return false;
    valley_height = reference.peak.height;

    // Read a peak
    if(!(features >> reference.peak)) return false;
    reference.prominence = reference.peak.height - valley_height;

    return true;
}


ProminenceReader::ProminenceReader(FeatureReader& features) :
  features(features), sea_level(0.0) {
    // Read the first reference peak
    if((features >> reference.peak) &&
       (features.last_type() == Peak || // Ignore a leading valley
        (features >> reference.peak))) {
        reference.prominence = reference.peak.height - sea_level;
        reference_good = true;
        reader_good = true;
    } else {
        reference_good = false;
        reader_good = false;
    }
}


ProminenceReader& ProminenceReader::operator>> (Prominence& p) {
    // Peaks on the stack are descending in height
    while(reference_good &&
          (partial.empty() ||
           partial.top().peak.height > reference.peak.height))
        reference_good = update_reference();

    // After reading all features, process the stack
    if(!reference_good) {
        if(!partial.empty()) {
            p = partial.top();
            partial.pop();
        } else reader_good = false; // Done
        return *this;
    }

    // We have a good reference peak, higher than the top of our stack
    double valley_height = reference.peak.height - reference.prominence;
    p = partial.top();
    partial.pop();

    if(p.prominence > p.peak.height - valley_height) {
        reference.prominence = reference.peak.height -
                               (p.peak.height - p.prominence);
        p.prominence = p.peak.height - valley_height;
    }

    return *this;
}


int main(int argc, char* argv[]) {
    FeatureReader features(cin);
    ProminenceReader prominences(features);
    set<Prominence> peaks;
    Prominence p;

    
    /* In case we want only the k highest prominences and k < log(n),
     * using a list instead of a set reduces the running time from O(log(n)*n)
     * to O(k*n). */
    while(prominences >> p)
        peaks.insert(p);

    for(Prominence p : peaks) {
        cout << "Peak at " << p.peak.position
             << " (height: " << p.peak.height
             << ") has prominence " << p.prominence << endl;
    }

    return 0;
}
