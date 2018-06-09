#include <iostream>
#include <set>
#include "greader.hpp"
#include "preader.hpp"

using namespace std;


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


int main(int argc, char* argv[]) {
    double threshold = sea_level;
    HeightMap map;

    switch (--argc) {
      case 2:
        threshold = stod(argv[1]);
      case 1:
        if (read_map(argv[argc], map)) break;
      default:
        usage();
        return 1;
    }

    ProminenceReader prominences(map, threshold);
    set<Prominence> peaks;
    Prominence p;

    while (prominences >> p)
        peaks.insert(p);

    for (auto p : peaks)
        cout << "Peak " << map[p.location].id
             << " (height: " << map[p.location].height
             << ") has prominence " << p.prominence << endl;

    return 0;
}
