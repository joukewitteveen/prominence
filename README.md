AllPeaksProminence
==================

These programs implement algorithms for computing a list of peaks in a
heightmap, sorted by prominence.


prominence1d
------------

This program operates on a one dimensional heightmap. The algorithm is an
online algorithm with a stack. For a heightmap with `m` points and `n` peaks,
its running time is in `O(m + n*log(n))` and its space usage is in `O(n)`.

**Input:**
A one dimensional heightmap presented as whitespace separated pairs of
*position* and *height*. The pairs should be presented in order of increasing
*position*. No empty lines are allowed.

**Output:**
All peaks, sorted by prominence.

### Building

Use `make` in the `prominence1d` directory.


prominence
----------

This program operates on arbitrary graphs. Graphs can be supplied as either
`GraphML` or `DOT` files, with heights specified in vertex attributes named
`height`. Alternatively, a digital elevation model can be specified as a `DEM`
file in a format understood by `GDAL`. Internally, the file will be converted
into a grid graph.

**Output:**
All peaks, sorted by prominence.

### Building

Use `make [all | prominence] [NOGDAL=1]` in the `prominence` directory.

A C++11-compatible compiler and the `Boost` headers are required for building
the program. Without the final option, also the GDAL libraries are needed.


minprominence
-------------

This program accepts the same input graphs as the `prominence` program. For a
given number of peaks, it finds a selection of that many peaks with a minimum
combined, or, equivalently, average, relative prominence.

**Output:**
A selection of peaks and their combined relative prominence.

### Building

Use `make [all | minprominence] [NOGDAL=1]` in the `prominence` directory.


___
This algorithm was conceived by *Rudolf Fleischer* in collaboration with
*Jouke Witteveen*, *Michael Emmerich*, and *Günter Rote* at the
*Fixed-Parameter Computational Geometry* workshop 2018, at the *Lorentz Center*
in *Leiden*.
