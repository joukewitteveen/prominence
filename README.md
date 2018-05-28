AllPeaksProminence
------------------

This program implements an online algorithm for computing a list of peaks in a
one dimensional heightmap, sorted by prominence. For a heightmap with `m`
points and `n` peaks, the running time of the algorithm is in `O(m + n*log(n))`
and the space usage is in `O(n)`.

**Input:**
A one dimensional heightmap presented as whitespace separated pairs of
*position* and *height*. The pairs should be presented in order of increasing
*position*. No empty lines are allowed.

**Output:**
All peaks, sorted by prominence.


This algorithm was conceived by *Rudolf Fleischer* in collaboration with
*Jouke Witteveen*, *Michael Emmerich*, and *Günter Rote* at the
*Fixed-Parameter Computational Geometry* workshop 2018, at the *Lorentz Center*
in *Leiden*.
