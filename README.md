# VolProc ~ MP2RAGE Volume Processor

## Requirements
- Properly defined parameters file in `JSON` format (cf. `example.json`)

## Hard-coded choices in code base
- No extrapolation: points beyond interpolant's definition are set to `0.`
- Propagation of the extrapolation mask to derived volumes except for Synthetic EDGE and its denoised version
- Geometric linear interpolation from a Delaunay triangulation mesh as $(B_1, \: T_1\text{-w UNI}) \rightarrow qT_1$ interpolator
- No rescaling of inputs in interpolator
- $qT_1$ and time quantities are expected to be given in $ms$ for synthetic volume computation to play well
- All volumes are rounded to nearest integer before export to `NIfTI` format
- $qR_1$ is given in per kilosecond ($ks^{-1}$) to limit information loss when rounding to nearest integer
- Outputs are given in:
    - $qT_1$ $\rightarrow$ $ms$
    - $qR_1$ $\rightarrow$ $ks^{-1}$
    - if `use_deprecated_normalization_from_12bits: true`:
        - $B_1$-corrected $T_1$-w UNI $\rightarrow$ $[0, \: 4096]$
        - Synthetic EDGE $\rightarrow$ $[0, \: 4096]$
        - Synthetic FLAWS $\rightarrow$ $[0, \: 4096]$
        - Denoised $T_1$-w UNI $\rightarrow$ $[0, \: 4096]$
        - Denoised $B_1$-corrected $T_1$-w UNI $\rightarrow$ $[0, \: 4096]$
        - Denoised Synthetic EDGE $\rightarrow$ $[0, \: 4096]$
        - Denoised Synthetic FLAWS $\rightarrow$ $[0, \: 4096]$
    - else if `use_deprecated_normalization_from_12bits: false`:
        - $B_1$-corrected $T_1$-w UNI $\rightarrow$ $[0, \: 4095]$
        - Synthetic EDGE $\rightarrow$ $[0, \: 4095]$
        - Synthetic FLAWS $\rightarrow$ $[0, \: 4095]$
        - Denoised $T_1$-w UNI $\rightarrow$ $[0, \: 4095]$
        - Denoised $B_1$-corrected $T_1$-w UNI $\rightarrow$ $[0, \: 4095]$
        - Denoised Synthetic EDGE $\rightarrow$ $[0, \: 4095]$
        - Denoised Synthetic FLAWS $\rightarrow$ $[0, \: 4095]$
- Denoising of ... uses ... range for denoising algorithm:
    - $T_1$-w UNI $\rightarrow$ $[-0.5, \: 0.5]$
    - $B_1$-corrected $T_1$-w UNI $\rightarrow$ $[-0.5, \: 0.5]$
    - if `use_denoising_on_unit_normed_synthetic_maps: true`:
        - Synthetic EDGE $\rightarrow$ $[-0.5, \: 0.5]$
        - Synthetic FLAWS $\rightarrow$ $[-0.5, \: 0.5]$
    - else if `use_denoising_on_unit_normed_synthetic_maps: false`:
        - if `use_deprecated_normalization_from_12bits: true`:
            - Synthetic EDGE $\rightarrow$ $[0, \: 4096]$
            - Synthetic FLAWS $\rightarrow$ $[0, \: 4096]$
        - else if `use_deprecated_normalization_from_12bits: false`:
            - Synthetic EDGE $\rightarrow$ $[0, \: 4095]$
            - Synthetic FLAWS $\rightarrow$ $[0, \: 4095]$


## Features included
- MP2RAGE bijectivity's correction
- $qT_1$ computation
- $qR_1$ computation
- $B_1$-corrected $T_1$-w UNI computation
- Synthetic EDGE computation
- Synthetic FLAWS computation
- Masking outside valid $qT_1$ range for volumes derived from $qT_1$
- Normalization from/to `DICOM` 12bits range, unit cube ([-0.5, 0.5]), various units
- In-house denoising Algorithms
- ANTs' volume transformation to reference (e.g. bringing $B_1$ to $T_1$-w UNI space)
- ANTs' Gaussian smoothing
- Various utilities, templating and overloads
- Multithreading

## Still needs
- Fix `boost`'s timer library static linking (#1)
- Proper `CMakeLists.txt` / `Makefile` with support for tests / release / debug / example (#2)
- Test suite (support for windows / linux) (#3)
- Documentation (#4)
- Copyright and Credits header in files (#5)
- README / Wiki (#4, #6)


## Misc
### Compilation dependencies
- C++ $\geq$ 17
- Boost (https://github.com/boostorg/boost)
- Eigen3 (https://gitlab.com/libeigen/eigen)
- ANTs (https://github.com/ANTsX/ANTs)
- RNifti (https://github.com/jonclayden/RNifti)
- libinterpolate (https://github.com/CD3/libInterpolate)
- make / g++

### Runtime dependencies
- `libboost_timer.so.1.83.0` (Boost Timer shared library to measure performance / elapsed time)

### Usage
`$ {ROOTDIR}/bin/main {ROOTDIR}/example.json`

_This is a header-only library so integrating it to existing C++ code should be as trivial as adding an `#include` to the proper header files (assuming the source dependencies are met)._

### Tested for exact reproduction of previous code
- [x] $qT_1$
- [x] $qR_1$
- [x] $B_1$-corrected $T_1$-w UNI
- [x] $B_1$-corrected $T_1$-w UNI denoised
- [x] $T_1$-w UNI denoised
- [x] Synthetic EDGE
- [x] Synthetic EDGE denoised
- [x] Synthetic FLAWS
- [x] Synthetic FLAWS denoised

### Future
- GUI?
- @amasliash's synthetic EDGE / synthetic FLAWS parameters optimizer?