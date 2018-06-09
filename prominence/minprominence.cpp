#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <type_traits>
#include "greader.hpp"
#include "preader.hpp"

using namespace std;


typedef typename vector<double>::size_type Size;


/* Minimum total relative prominence is computed using dynamic programming.
 * Optimal selections of peaks are computed inside contour lines. */
class MinProminence {
    struct SubTree {
        double key_height;
        vector<vector<Location>> selections;
        vector<double> totals;
        SubTree(Location const l, double const h) :
          key_height(h),
          selections(2, vector<Location>()),
          totals(2, 0.0) {
            selections.back().push_back(l);
        }
    };
    typedef unique_ptr<SubTree> SubTree_;
    HeightMap const& map;
    vector<SubTree_> partials;
    Size const target_size;
    SubTree_& get(Location);
    void merge(SubTree_&, SubTree_&);
    void update_key_height(SubTree_&, double);
  public:
    MinProminence(HeightMap const& map, Size const size) :
      map(map), partials(num_vertices(map)), target_size(size) {}
    MinProminence& operator<< (Prominence const&);
    double yield(vector<Location>&); // Destructively determine best selection
};


MinProminence::SubTree_& MinProminence::get(Location l) {
    static_assert(is_integral<Location>::value,
                  "Locations must be usable as indices.");
    if (!partials[l])
        partials[l] = SubTree_(new SubTree(l, map[l].height));
    return partials[l];
}


void MinProminence::merge(SubTree_& target, SubTree_& source) {
    Size const max_target = target->totals.size() - 1,
               max_source = source->totals.size() - 1,
               max_post = min(max_target + max_source, target_size);
    vector<Size> splits(max_post + 1);
    vector<double> best(max_post + 1, 0.0);
    double candidate;

    // Determine optimal combinations of selections
    for (Size k = 1, split, min_split; k <= max_post; ++k) {
        split = min(max_target, k);
        // We cannot use max() because the variables are unsigned
        min_split = k > max_source ? k - max_source : 0;
        splits[k] = split;
        best[k] = target->totals[split] + source->totals[k - split];
        while (split-- > min_split) {
            candidate = target->totals[split] + source->totals[k - split];
            if (candidate < best[k]) {
                splits[k] = split;
                best[k] = candidate;
            }
        }
    }

    // The merger must be carried out backwards
    target->selections.resize(max_post + 1);
    target->totals = best;
    for (Size k = max_post; k >= 1; --k) {
        if (k == splits[k]) continue;

        auto& selection = target->selections[k];
        selection = target->selections[splits[k]];
        selection.insert(selection.end(),
                         source->selections[k - splits[k]].begin(),
                         source->selections[k - splits[k]].end());
    }

    source.reset(nullptr);
}


void MinProminence::update_key_height(SubTree_& tree, double key_height) {
    double const delta = tree->key_height - key_height;

    // Do not increase the total prominence of the empty selection
    /* If we would also not increase the total prominence of a selection of
     * target_size locations, we would obtain a localized total prominence. */
    for_each (tree->totals.begin() + 1, tree->totals.end(),
              [&](double& total) { total += delta; });

    tree->key_height = key_height;
}


MinProminence& MinProminence::operator<< (Prominence const& p) {
    Location child = p.location,
             parent = map[child].parent;
    double key_height = map[child].height - p.prominence;
    SubTree_& target = get(parent);

    update_key_height(target, key_height);

    if (child != parent) {
        SubTree_& source = get(child);
        update_key_height(source, key_height);
        merge(target, source);
    }

    return *this;
}


double MinProminence::yield(vector<Location>& selection) {
    double prominence = 0.0;
    auto nonempty = [](SubTree_ const& t) { return t.operator bool (); };
    auto end = partials.end();
    auto target = find_if(partials.begin(), end, nonempty);
    auto source = target;

    if (target != end) {
        while ((source = find_if(source + 1, end, nonempty)) != end)
            merge(*target, *source);

        selection = (*target)->selections.back();
        prominence = (*target)->totals.back();
        target->reset(nullptr);
    }

    return prominence;
}


void usage() {
    cout << "Usage: minprominence <size> <file>" << endl << endl
         << "Supported file formats:" << endl
         << "  DOT     (.gv, .dot)" << endl
         << "  GraphML (.graphml, .xml)" << endl
#ifndef NOGDAL
         << "  DEM     (.dem, ...)" << endl
#endif //NOGDAL
         ;
}


int main(int argc, char* argv[]) {
    HeightMap map;
    Size size;

    switch (--argc) {
      case 2:
        size = stoul(argv[argc - 1]);
        if (read_map(argv[argc], map)) break;
      default:
        usage();
        return 1;
    }

    ProminenceReader prominences(map);
    MinProminence accumulator(map, size);
    Prominence p;
    vector<Location> selection;
    double prominence;

    while (prominences >> p)
        accumulator << p;

    prominence = accumulator.yield(selection);

    cout << "A relative prominence of " << prominence
         << " is obtained with " << selection.size() << " peaks:" << endl;
    for (auto l : selection)
        cout << "Peak " << map[l].id
             << " (height: " << map[l].height << ")" << endl;

    return 0;
}
