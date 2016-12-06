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
