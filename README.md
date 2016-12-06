sndfilter
=========

Algorithms for simple sound filters, like lowpass, highpass, notch, etc.

It's easy to find countless math equations on sound filters, but a bit harder to find simple source
code.  This library is my attempt at cleaning up and presenting the math-heavy filter algorithms for
the programming community.

Build Instructions
------------------

The `./build` script is a simple bash script that compiles the source files using `clang`.  Each
`.c` file is compiled to a `.o`, then linked together.  There really isn't anything too crazy here.

Simply run `./build` and the executable should be `./tgt/sndfilter`.

Filters
-------

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

The [biquad.c](https://github.com/voidqk/sndfilter/blob/master/src/biquad.c) file is the meat of the
processing.  There are some comments in that file that describe what is happening.

I do not understand the math, so please don't ask me any questions :-).  The core formulas were
extracted from the [Chromium](https://github.com/nwjs/chromium.src) source code (specifically
[Biquad.cpp](https://github.com/nwjs/chromium.src/blob/df7f8c8582b9a78c806a7fa1e9d3f3ba51f7a698/third_party/WebKit/Source/platform/audio/Biquad.cpp),
and cleaned up to make it easier to read.
