
sndfilter
=========

Algorithms for sound filters, like reverb, dynamic range compression, lowpass, highpass, notch, etc.

It's easy to find countless math equations on sound filters, but a bit harder to find simple source
code.  This library is my attempt at cleaning up and presenting the math-heavy filter algorithms for
the programming community.

Please note that I favored simple code over fast code.  Hopefully it's made it more understandable.

(MIT License)

Build Instructions
------------------

The `./build` script is a simple bash script that compiles the source files using `clang`.  It's
dirt simple, [I promise](https://github.com/velipso/sndfilter/blob/master/build).

Simply run `./build` and the executable should be `./tgt/sndfilter`.

### C++ Support

This project is pure C, but I've left PRs open for those who want C++ support.  Check them out, they
might save you some time:

* [C++ headers](https://github.com/velipso/sndfilter/pull/18)
* [Meson build system support](https://github.com/velipso/sndfilter/pull/19)

Filters
-------

* [Reverb](https://en.wikipedia.org/wiki/Reverberation) (Algorithmic)
* [Compressor](https://en.wikipedia.org/wiki/Dynamic_range_compression)
* [Low-Pass](https://en.wikipedia.org/wiki/Low-pass_filter) (Cutoff, Resonance)
* [High-Pass](https://en.wikipedia.org/wiki/High-pass_filter) (Cutoff, Resonance)
* [Band-Pass](https://en.wikipedia.org/wiki/Band-pass_filter) (Frequency, Q)
* [Notch/Band-stop](https://en.wikipedia.org/wiki/Band-stop_filter) (Frequency, Q)
* [Peaking](https://en.wikipedia.org/wiki/Equalization_(audio)) (Frequency, Q, Gain)
* [All-Pass](https://en.wikipedia.org/wiki/All-pass_filter) (Frequency, Q)
* [Low Shelf](http://www.audiorecording.me/what-is-a-low-shelf-and-high-shelf-filter-in-parametric-equalization.html) (Frequency, Q, Gain)
* [High Shelf](http://www.audiorecording.me/what-is-a-low-shelf-and-high-shelf-filter-in-parametric-equalization.html) (Frequency, Q, Gain)

Implementation
--------------

The [reverb.c](https://github.com/velipso/sndfilter/blob/master/src/reverb.c),
[compressor.c](https://github.com/velipso/sndfilter/blob/master/src/compressor.c), and
[biquad.c](https://github.com/velipso/sndfilter/blob/master/src/biquad.c) are the core
algorithms.

I do not understand the biquad math, so please don't ask me any questions :-).  The core formulas
were extracted from
[Biquad.cpp](https://github.com/nwjs/chromium.src/blob/df7f8c8582b9a78c806a7fa1e9d3f3ba51f7a698/third_party/WebKit/Source/platform/audio/Biquad.cpp)
(Chromium source), and cleaned up a bit to make easier to read.

The compressor came from
[DynamicsCompressorKernel.cpp](https://github.com/nwjs/chromium.src/blob/df7f8c8582b9a78c806a7fa1e9d3f3ba51f7a698/third_party/WebKit/Source/platform/audio/DynamicsCompressorKernel.cpp)
(also from Chromium), and cleaned up a bit more.  I swapped out the adaptive release curve and
simplified the knee calculations.  I feel a little more comfortable with that algorithm because
there isn't a whole lot of magical math involved.

The reverb effect is a complete rewrite of [Freeverb3](http://www.nongnu.org/freeverb3/)'s
Progenitor2 algorithm.  It took quite a lot of effort to tear apart the algorithm and rebuild
it, but I'm pretty sure it's right.
