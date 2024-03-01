# MP2RAGE_Volume_Processor

# Quirks
## Requirements
- $B_1$ volume pre-registered on the $T_1$-w UNI volume
- Properly defined parameters file in `JSON` format (cf. `example.json`)

## Choices in main script
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
    - $B_1$-corrected $T_1$-w UNI $\rightarrow$ $[0, \: 4096]$
    - Synthetic EDGE $\rightarrow$ $[0, \: 4096]$
    - Synthetic FLAWS $\rightarrow$ $[0, \: 4096]$
    - Denoised $T_1$-w UNI $\rightarrow$ $[0, \: 4096]$
    - Denoised $B_1$-corrected $T_1$-w UNI $\rightarrow$ $[0, \: 4096]$
    - Denoised Synthetic EDGE $\rightarrow$ $[0, \: 4096]$
    - Denoised Synthetic FLAWS $\rightarrow$ $[0, \: 4096]$
- Denoising of ... uses ... version of denoising algorithm:
    - $T_1$-w UNI $\rightarrow$ $[-0.5, \: 0.5]$ range
    - $B_1$-corrected $T_1$-w UNI $\rightarrow$ $[-0.5, \: 0.5]$ range
    - Synthetic EDGE $\rightarrow$ $[0, \: 4096]$ range
    - Synthetic FLAWS $\rightarrow$ $[0, \: 4096]$ range

## Includes
- $qT_1$ computation
- $qR_1$ computation
- $B_1$-corrected $T_1$-w UNI computation
- Synthetic EDGE computation
- Synthetic FLAWS computation
- Denoising
- Normalization from/to `DICOM` 12bits range, unit cube ([-0.5, 0.5]), various units
- MP2RAGE bijectivity's correction
- Multithreading
- Various utilities, templating and overloads

## Does not include
- $B_1$ registration on $T_1$-w UNI,
- $B_1$ smoothing,
- masking outside valid $qT_1$ range
- `print_usage` function

## Still needs
- Proper `CMakeLists.txt` / `Makefile` with support for tests / release / debug / example
- Test suite (support for windows / linux)
- Documentation
- Copyright and Credits header in files
- README / Wiki


# Misc
## Compilation dependencies
- C++ $\geq$ 17
- Boost (https://github.com/boostorg/boost)
- Eigen3 (https://gitlab.com/libeigen/eigen)
- RNifti (https://github.com/jonclayden/RNifti)
- libinterpolate (https://github.com/CD3/libInterpolate)
- make / g++

## Runtime dependencies
- None (should be statically built)

## Usage
`$ {ROOTDIR}/bin/main {ROOTDIR}/example.json`

_This is a header-only library so integrating it to existing C++ code should be as trivial as adding an `#include# to the proper header files (assuming the source dependencies are met)._

## Future
- [breaking change] Stop using deprecated `TO_12BITS_RANGE_DEPRECATED` and `TO_UNI_RANGE_DEPRECATED` normalizations (erroneous [0, 4096] range)?
- GUI?
- @amasliash's synthetic EDGE / synthetic FLAWS parameters optimizer?

## Tested for exact reproduction of previous code
- [x] $qT_1$
- [x] $qR_1$
- [x] $B_1$-corrected $T_1$-w UNI
- [x] $B_1$-corrected $T_1$-w UNI denoised
- [x] $T_1$-w UNI denoised
- [x] Synthetic EDGE
- [x] Synthetic EDGE denoised
- [x] Synthetic FLAWS
- [x] Synthetic FLAWS denoised