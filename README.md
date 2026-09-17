# KeyboardSIm

Numerical simulation of vibration and resonance on a 2D discretized plate,
built to investigate whether measurable physical properties can explain the
subjective descriptors used to talk about keyboard sound, in other words a mathematical proxy for 'thock' 

Phase 1 is a CPU-only 2D wave-equation solver in C++, with numerical
verification against analytical standing waves. Later phases upgrade to the
Kirchhoff plate equation, port the hot loop to CUDA, add FFT-based modal
analysis, and build keyboard-specific configurations.


## Build

Phase 1 toolchain: UCRT64 GCC (MSYS2) + Ninja + CMake, on Windows 11.
CUDA in Phase 5 will require MSVC as the host compiler on Windows; the
build system is set up to accept either compiler for the CPU code.

```
cmake --preset default
cmake --build --preset default
ctest --preset default
```
