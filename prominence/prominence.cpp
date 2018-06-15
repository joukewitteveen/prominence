#include <iostream>
#include <iomanip>
#include <set>
#include "heightmap.hpp"
#include "preader.hpp"

using namespace std;


void usage() {
    cout << "Usage: prominence [threshold] <file>" << endl << endl
         << "threshold defaults to " << sea_level << "." << endl << endl
         << "Supported file formats:" << endl;
    for (auto& format : supported_formats())
        cout << "  " << setw(10) << left << format[0] << format[1] << endl;
}


int main(int argc, char* argv[]) {
    Height threshold = sea_level;
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
        cout << "Peak " << get_name(p.location, map)
             << " (height: " << map[p.location].height
             << ") has prominence " << p.prominence << endl;

    return 0;
}
