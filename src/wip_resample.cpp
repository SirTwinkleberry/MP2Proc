#include <iostream>

#include "externals/include/RNifti.h"


static RNifti::NiftiImage RESAMPLE_FROM_TO(RNifti::NiftiImage from_img, RNifti::NiftiImage to_img, int order = 3, std::string mode = "constant", double cval = 0.0)
{
    RNifti::NiftiImage out_image = RNifti::NiftiImage(to_img);
    std::vector<double> from_data = from_img.getData<double>();



    std::vector<double> from_resampled = from_data;
    out_image.replaceData(from_resampled);
    return out_image;
}

static void GEOMETRIC_TRANSFORM()
{
    // int
    // NI_GeometricTransform(PyArrayObject *input, int (*map)(npy_intp*, double*,
    //                 int, int, void*), void* map_data, PyArrayObject* matrix_ar,
    //                 PyArrayObject* shift_ar, PyArrayObject *coordinates,
    //                 PyArrayObject *output, int order, int mode, double cval,
    //                 int nprepad)
    // {
    //     char *po, *pi, *pc = NULL;
    //     npy_intp **edge_offsets = NULL, **data_offsets = NULL, filter_size;
    //     char **edge_grid_const = NULL;
    //     npy_intp ftmp[NPY_MAXDIMS], *fcoordinates = NULL, *foffsets = NULL;
    //     npy_intp cstride = 0, kk, hh, ll, jj;
    //     npy_intp size;
    //     double **splvals = NULL, icoor[NPY_MAXDIMS];
    //     npy_intp idimensions[NPY_MAXDIMS], istrides[NPY_MAXDIMS];
    //     NI_Iterator io, ic;
    //     npy_double *matrix = matrix_ar ? (npy_double*)PyArray_DATA(matrix_ar) : NULL;
    //     npy_double *shift = shift_ar ? (npy_double*)PyArray_DATA(shift_ar) : NULL;
    //     int irank = 0, orank, spline_mode;
    //     NPY_BEGIN_THREADS_DEF;

    //     NPY_BEGIN_THREADS;

    //     for(kk = 0; kk < PyArray_NDIM(input); kk++) {
    //         idimensions[kk] = PyArray_DIM(input, kk);
    //         istrides[kk] = PyArray_STRIDE(input, kk);
    //     }
    //     irank = PyArray_NDIM(input);
    //     orank = PyArray_NDIM(output);

    //     /* if the mapping is from array coordinates: */
    //     if (coordinates) {
    //         /* initialize a line iterator along the first axis: */
    //         if (!NI_InitPointIterator(coordinates, &ic))
    //             goto exit;
    //         cstride = ic.strides[0];
    //         if (!NI_LineIterator(&ic, 0))
    //             goto exit;
    //         pc = (void *)(PyArray_DATA(coordinates));
    //     }

    //     /* offsets used at the borders: */
    //     edge_offsets = malloc(irank * sizeof(npy_intp*));
    //     data_offsets = malloc(irank * sizeof(npy_intp*));
    //     if (NPY_UNLIKELY(!edge_offsets || !data_offsets)) {
    //         NPY_END_THREADS;
    //         PyErr_NoMemory();
    //         goto exit;
    //     }

    //     if (mode == NI_EXTEND_GRID_CONSTANT) {
    //         // boolean indicating if the current point in the filter footprint is
    //         // outside the bounds
    //         edge_grid_const = malloc(irank * sizeof(char*));
    //         if (NPY_UNLIKELY(!edge_grid_const)) {
    //             NPY_END_THREADS;
    //             PyErr_NoMemory();
    //             goto exit;
    //         }
    //         for(jj = 0; jj < irank; jj++)
    //             edge_grid_const[jj] = NULL;
    //         for(jj = 0; jj < irank; jj++) {
    //             edge_grid_const[jj] = malloc((order + 1) * sizeof(char));
    //             if (NPY_UNLIKELY(!edge_grid_const[jj])) {
    //                 NPY_END_THREADS;
    //                 PyErr_NoMemory();
    //                 goto exit;
    //             }
    //         }
    //     }

    //     for(jj = 0; jj < irank; jj++)
    //         data_offsets[jj] = NULL;
    //     for(jj = 0; jj < irank; jj++) {
    //         data_offsets[jj] = malloc((order + 1) * sizeof(npy_intp));
    //         if (NPY_UNLIKELY(!data_offsets[jj])) {
    //             NPY_END_THREADS;
    //             PyErr_NoMemory();
    //             goto exit;
    //         }
    //     }
    //     /* will hold the spline coefficients: */
    //     splvals = malloc(irank * sizeof(double*));
    //     if (NPY_UNLIKELY(!splvals)) {
    //         NPY_END_THREADS;
    //         PyErr_NoMemory();
    //         goto exit;
    //     }
    //     for(jj = 0; jj < irank; jj++)
    //         splvals[jj] = NULL;
    //     for(jj = 0; jj < irank; jj++) {
    //         splvals[jj] = malloc((order + 1) * sizeof(double));
    //         if (NPY_UNLIKELY(!splvals[jj])) {
    //             NPY_END_THREADS;
    //             PyErr_NoMemory();
    //             goto exit;
    //         }
    //     }

    //     filter_size = 1;
    //     for(jj = 0; jj < irank; jj++)
    //         filter_size *= order + 1;

    //     /* initialize output iterator: */
    //     if (!NI_InitPointIterator(output, &io))
    //         goto exit;

    //     /* get data pointers: */
    //     pi = (void *)PyArray_DATA(input);
    //     po = (void *)PyArray_DATA(output);

    //     /* make a table of all possible coordinates within the spline filter: */
    //     fcoordinates = malloc(irank * filter_size * sizeof(npy_intp));
    //     /* make a table of all offsets within the spline filter: */
    //     foffsets = malloc(filter_size * sizeof(npy_intp));
    //     if (NPY_UNLIKELY(!fcoordinates || !foffsets)) {
    //         NPY_END_THREADS;
    //         PyErr_NoMemory();
    //         goto exit;
    //     }
    //     for(jj = 0; jj < irank; jj++)
    //         ftmp[jj] = 0;
    //     kk = 0;
    //     for(hh = 0; hh < filter_size; hh++) {
    //         for(jj = 0; jj < irank; jj++)
    //             fcoordinates[jj + hh * irank] = ftmp[jj];
    //         foffsets[hh] = kk;
    //         for(jj = irank - 1; jj >= 0; jj--) {
    //             if (ftmp[jj] < order) {
    //                 ftmp[jj]++;
    //                 kk += istrides[jj];
    //                 break;
    //             } else {
    //                 ftmp[jj] = 0;
    //                 kk -= istrides[jj] * order;
    //             }
    //         }
    //     }

    //     spline_mode = _get_spline_boundary_mode(mode);

    //     size = PyArray_SIZE(output);
    //     for(kk = 0; kk < size; kk++) {
    //         double t = 0.0;
    //         int constant = 0, edge = 0;
    //         npy_intp offset = 0;
    //         if (mode == NI_EXTEND_GRID_CONSTANT) {
    //             // reset edge flags for each location in the filter footprint
    //             for (hh = 0; hh < irank; hh++) {
    //                 for(ll = 0; ll <= order; ll++) {
    //                     edge_grid_const[hh][ll] = 0;
    //                 }
    //             }
    //         }
    //         if (map) {
    //             NPY_END_THREADS;
    //             /* call mappint functions: */
    //             if (!map(io.coordinates, icoor, orank, irank, map_data)) {
    //                 if (!PyErr_Occurred())
    //                     PyErr_SetString(PyExc_RuntimeError,
    //                                     "unknown error in mapping function");
    //                 goto exit;
    //             }
    //             NPY_BEGIN_THREADS;
    //         } else if (matrix) {
    //             /* do an affine transformation: */
    //             npy_double *p = matrix;
    //             for(hh = 0; hh < irank; hh++) {
    //                 icoor[hh] = 0.0;
    //                 for(ll = 0; ll < orank; ll++)
    //                     icoor[hh] += io.coordinates[ll] * *p++;
    //                 icoor[hh] += shift[hh];
    //             }
    //         } else if (coordinates) {
    //             /* mapping is from an coordinates array: */
    //             char *p = pc;
    //             switch (PyArray_TYPE(coordinates)) {
    //                 CASE_MAP_COORDINATES(NPY_BOOL, npy_bool,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_UBYTE, npy_ubyte,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_USHORT, npy_ushort,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_UINT, npy_uint,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_ULONG, npy_ulong,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_ULONGLONG, npy_ulonglong,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_BYTE, npy_byte,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_SHORT, npy_short,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_INT, npy_int,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_LONG, npy_long,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_LONGLONG, npy_longlong,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_FLOAT, npy_float,
    //                                     p, icoor, irank, cstride);
    //                 CASE_MAP_COORDINATES(NPY_DOUBLE, npy_double,
    //                                     p, icoor, irank, cstride);
    //             default:
    //                 NPY_END_THREADS;
    //                 PyErr_SetString(PyExc_RuntimeError,
    //                                 "coordinate array data type not supported");
    //                 goto exit;
    //             }
    //         }

    //         /* iterate over axes: */
    //         for(hh = 0; hh < irank; hh++) {
    //             double cc = icoor[hh] + nprepad;
    //             if ((mode != NI_EXTEND_GRID_CONSTANT) && (mode != NI_EXTEND_NEAREST)) {
    //                 /* if the input coordinate is outside the borders, map it: */
    //                 cc = map_coordinate(cc, idimensions[hh], mode);
    //             }
    //             if (cc > -1.0 || mode == NI_EXTEND_GRID_CONSTANT || mode == NI_EXTEND_NEAREST) {
    //                 /* find the filter location along this axis: */
    //                 npy_intp start;
    //                 if (order & 1) {
    //                     start = (npy_intp)floor(cc) - order / 2;
    //                 } else {
    //                     start = (npy_intp)floor(cc + 0.5) - order / 2;
    //                 }
    //                 /* get the offset to the start of the filter: */
    //                 offset += istrides[hh] * start;
    //                 npy_intp idx = 0;

    //                 if (mode == NI_EXTEND_GRID_CONSTANT) {
    //                     // Determine locations in the filter footprint that are
    //                     // outside the range.
    //                     for(ll = 0; ll <= order; ll++) {
    //                         idx = start + ll;
    //                         edge_grid_const[hh][ll] = (idx < 0 || idx >= idimensions[hh]);
    //                     }
    //                 } else {

    //                     if (start < 0 || start + order >= idimensions[hh]) {
    //                         /* implement border mapping, if outside border: */
    //                         edge = 1;
    //                         edge_offsets[hh] = data_offsets[hh];

    //                         for(ll = 0; ll <= order; ll++) {
    //                             idx = start + ll;
    //                             idx = (npy_intp)map_coordinate(idx, idimensions[hh], spline_mode);

    //                             /* calculate and store the offsets at this edge: */
    //                             edge_offsets[hh][ll] = istrides[hh] * (idx - start);
    //                         }
    //                     } else {
    //                         /* we are not at the border, use precalculated offsets: */
    //                         edge_offsets[hh] = NULL;
    //                     }
    //                 }
    //                 get_spline_interpolation_weights(cc, order, splvals[hh]);
    //             } else {
    //                 /* we use the constant border condition: */
    //                 constant = 1;
    //                 break;
    //             }
    //         }

    //         if (!constant) {
    //             npy_intp *ff = fcoordinates;
    //             const int type_num = PyArray_TYPE(input);
    //             t = 0.0;
    //             for(hh = 0; hh < filter_size; hh++) {
    //                 double coeff = 0.0;
    //                 npy_intp idx = 0;
    //                 char is_cval = 0;
    //                 if (mode == NI_EXTEND_GRID_CONSTANT) {
    //                     for(ll = 0; ll < irank; ll++) {
    //                         if (edge_grid_const[ll][ff[ll]]) {
    //                             is_cval = 1;
    //                         }
    //                     }
    //                 }
    //                 if (is_cval) {
    //                     coeff = cval;
    //                 } else {
    //                     if (NPY_UNLIKELY(edge)) {
    //                         for(ll = 0; ll < irank; ll++) {
    //                             if (edge_offsets[ll])
    //                                 idx += edge_offsets[ll][ff[ll]];
    //                             else
    //                                 idx += ff[ll] * istrides[ll];
    //                         }
    //                     } else {
    //                         idx = foffsets[hh];
    //                     }
    //                     idx += offset;
    //                     switch (type_num) {
    //                         CASE_INTERP_COEFF(NPY_BOOL, npy_bool,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_UBYTE, npy_ubyte,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_USHORT, npy_ushort,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_UINT, npy_uint,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_ULONG, npy_ulong,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_ULONGLONG, npy_ulonglong,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_BYTE, npy_byte,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_SHORT, npy_short,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_INT, npy_int,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_LONG, npy_long,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_LONGLONG, npy_longlong,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_FLOAT, npy_float,
    //                                         coeff, pi, idx);
    //                         CASE_INTERP_COEFF(NPY_DOUBLE, npy_double,
    //                                         coeff, pi, idx);
    //                     default:
    //                         NPY_END_THREADS;
    //                         PyErr_SetString(PyExc_RuntimeError,
    //                                         "data type not supported");
    //                         goto exit;
    //                     }
    //                 }
    //                 /* calculate the interpolated value: */
    //                 for(ll = 0; ll < irank; ll++)
    //                     if (order > 0)
    //                         coeff *= splvals[ll][ff[ll]];
    //                 t += coeff;
    //                 ff += irank;
    //             }
    //         } else {
    //             t = cval;
    //         }
    //         /* store output value: */
    //         switch (PyArray_TYPE(output)) {
    //             CASE_INTERP_OUT(NPY_BOOL, npy_bool, po, t);
    //             CASE_INTERP_OUT_UINT(UBYTE, npy_ubyte, po, t);
    //             CASE_INTERP_OUT_UINT(USHORT, npy_ushort, po, t);
    //             CASE_INTERP_OUT_UINT(UINT, npy_uint, po, t);
    //             CASE_INTERP_OUT_UINT(ULONG, npy_ulong, po, t);
    //             CASE_INTERP_OUT_UINT(ULONGLONG, npy_ulonglong, po, t);
    //             CASE_INTERP_OUT_INT(BYTE, npy_byte, po, t);
    //             CASE_INTERP_OUT_INT(SHORT, npy_short, po, t);
    //             CASE_INTERP_OUT_INT(INT, npy_int, po, t);
    //             CASE_INTERP_OUT_INT(LONG, npy_long, po, t);
    //             CASE_INTERP_OUT_INT(LONGLONG, npy_longlong, po, t);
    //             CASE_INTERP_OUT(NPY_FLOAT, npy_float, po, t);
    //             CASE_INTERP_OUT(NPY_DOUBLE, npy_double, po, t);
    //         default:
    //             NPY_END_THREADS;
    //             PyErr_SetString(PyExc_RuntimeError, "data type not supported");
    //             goto exit;
    //         }
    //         if (coordinates) {
    //             NI_ITERATOR_NEXT2(io, ic, po, pc);
    //         } else {
    //             NI_ITERATOR_NEXT(io, po);
    //         }
    //     }

    // exit:
    //     NPY_END_THREADS;
    //     free(edge_offsets);
    //     if (edge_grid_const) {
    //         for(jj = 0; jj < irank; jj++)
    //             free(edge_grid_const[jj]);
    //         free(edge_grid_const);
    //     }
    //     if (data_offsets) {
    //         for(jj = 0; jj < irank; jj++)
    //             free(data_offsets[jj]);
    //         free(data_offsets);
    //     }
    //     if (splvals) {
    //         for(jj = 0; jj < irank; jj++)
    //             free(splvals[jj]);
    //         free(splvals);
    //     }
    //     free(foffsets);
    //     free(fcoordinates);
    //     return PyErr_Occurred() ? 0 : 1;
    // }
}


static void SCIPY()
{
    // if order < 0 or order > 5:
    //     raise RuntimeError('spline order not supported')
    // input = numpy.asarray(input)
    // if output_shape is None:
    //     if isinstance(output, numpy.ndarray):
    //         output_shape = output.shape
    //     else:
    //         output_shape = input.shape
    // if input.ndim < 1 or len(output_shape) < 1:
    //     raise RuntimeError('input and output rank must be > 0')
    // complex_output = numpy.iscomplexobj(input)
    // output = _ni_support._get_output(output, input, shape=output_shape,
    //                                  complex_output=complex_output)
    // if complex_output:
    //     kwargs = dict(offset=offset, output_shape=output_shape, order=order,
    //                   mode=mode, prefilter=prefilter)
    //     affine_transform(input.real, matrix, output=output.real,
    //                      cval=numpy.real(cval), **kwargs)
    //     affine_transform(input.imag, matrix, output=output.imag,
    //                      cval=numpy.imag(cval), **kwargs)
    //     return output
    // if prefilter and order > 1:
    //     padded, npad = _prepad_for_spline_filter(input, mode, cval)
    //     filtered = spline_filter(padded, order, output=numpy.float64,
    //                              mode=mode)
    // else:
    //     npad = 0
    //     filtered = input
    // mode = _ni_support._extend_mode_to_code(mode)
    // matrix = numpy.asarray(matrix, dtype=numpy.float64)
    // if matrix.ndim not in [1, 2] or matrix.shape[0] < 1:
    //     raise RuntimeError('no proper affine matrix provided')
    // if (matrix.ndim == 2 and matrix.shape[1] == input.ndim + 1 and
    //         (matrix.shape[0] in [input.ndim, input.ndim + 1])):
    //     if matrix.shape[0] == input.ndim + 1:
    //         exptd = [0] * input.ndim + [1]
    //         if not numpy.all(matrix[input.ndim] == exptd):
    //             msg = ('Expected homogeneous transformation matrix with '
    //                    'shape {} for image shape {}, but bottom row was '
    //                    'not equal to {}'.format(matrix.shape, input.shape, exptd))
    //             raise ValueError(msg)
    //     # assume input is homogeneous coordinate transformation matrix
    //     offset = matrix[:input.ndim, input.ndim]
    //     matrix = matrix[:input.ndim, :input.ndim]
    // if matrix.shape[0] != input.ndim:
    //     raise RuntimeError('affine matrix has wrong number of rows')
    // if matrix.ndim == 2 and matrix.shape[1] != output.ndim:
    //     raise RuntimeError('affine matrix has wrong number of columns')
    // if not matrix.flags.contiguous:
    //     matrix = matrix.copy()
    // offset = _ni_support._normalize_sequence(offset, input.ndim)
    // offset = numpy.asarray(offset, dtype=numpy.float64)
    // if offset.ndim != 1 or offset.shape[0] < 1:
    //     raise RuntimeError('no proper offset provided')
    // if not offset.flags.contiguous:
    //     offset = offset.copy()
    // if matrix.ndim == 1:
    //     warnings.warn(
    //         "The behavior of affine_transform with a 1-D "
    //         "array supplied for the matrix parameter has changed in "
    //         "SciPy 0.18.0.",
    //         stacklevel=2
    //     )
    //     _nd_image.zoom_shift(filtered, matrix, offset/matrix, output, order,
    //                          mode, cval, npad, False)
    // else:
    //     _nd_image.geometric_transform(filtered, None, None, matrix, offset,
    //                                   output, order, mode, cval, npad, None,
    //                                   None)
    // return output
}


int main(int argc, char const *argv[])
{
    /* Not taking pixel spacing into account atm */

    RNifti::NiftiImage from_img = RNifti::NiftiImage("/mnt/c/Users/tanderson/Desktop/b1Correction/cpp/preresliced_01_0008_tfl-b1map-sag-3mm-tar90_c32.nii.gz");
    RNifti::NiftiImage to_img = RNifti::NiftiImage("/mnt/c/Users/tanderson/Desktop/b1Correction/in/01_0025_t1-mp2r-sag-0p6-p3-cemerem-uni-images_c32.nii.gz");

    // for ( const auto &e : from_img.dim() )
    //     std::cout << e << " ";
    // std::cout << std::endl;
    // for ( const auto &e : to_img.dim() )
    //     std::cout << e << " ";
    // std::cout << std::endl;

    int from_nDim = from_img.nDims();
    int to_nDim = to_img.nDims();

    if ( from_nDim != 3 ) throw std::runtime_error("from_img must be 3-dimensional");
    if ( to_nDim != 3 ) throw std::runtime_error("to_img must be 3-dimensional");

    RNifti::NiftiImage::Xform::Matrix from_4x4Affine = RNifti::NiftiImage::Xform::Matrix(from_img.xform().matrix());
    RNifti::NiftiImage::Xform::Matrix to_4x4Affine = RNifti::NiftiImage::Xform::Matrix(to_img.xform().matrix());

    // RNifti::NiftiImage::Xform::Vector3 from_3x1Trans = from_img.xform().offset();
    // RNifti::NiftiImage::Xform::Vector3 to_3x1Trans = to_img.xform().offset();
    // RNifti::NiftiImage::Xform::Submatrix from_3x3Deform = from_img.xform().submatrix();
    // RNifti::NiftiImage::Xform::Submatrix to_3x3Deform = to_img.xform().submatrix();
    // RNifti::NiftiImage::Xform::Vector3 from_3x1PixSpacing = from_img.xform().spacing();
    // RNifti::NiftiImage::Xform::Vector3 to_3x1PixSpacing = to_img.xform().spacing();
    // a_to_affine = adapt_affine(to_4x4Affine, to_nDim) // Useless cause we're in 3D
    // a_from_affine = adapt_affine(from_4x4Affine, from_nDim) // Useless cause we're in 3D
    
    RNifti::NiftiImage::Xform::Matrix to_vox2from_vox = from_4x4Affine.inverse() * to_4x4Affine;

    RNifti::NiftiImage::Xform::Submatrix deform = RNifti::NiftiImage::Xform::Submatrix();
    RNifti::NiftiImage::Xform::Vector3 trans = RNifti::NiftiImage::Xform::Vector3();

    for ( int i = 0 ; i < 3 ; ++i )
    {
        for ( int j = 0 ; j < 4 ; ++j )
        {
            if ( j < 3 ) deform(i, j) = to_vox2from_vox(i, j);
            else trans[i] = to_vox2from_vox(i, j);
        }
    }

    // std::string fullMat = "";
    // int i = 0;
    // for ( const auto &elem : deform ) {
    //     fullMat += std::to_string(elem) + " ";
    //     if ((i+1)%3 == 0) fullMat += "\n";
    //     ++i;
    // }
    // std::cout << fullMat << std::endl;
    // std::cout << "[" << trans[0] << ", " << trans[1] << ", " << trans[2] << "]" << std::endl;

    /*
        std::vector<double> resampled_data = std::vector<double>(to_img.nVoxels());
    */

    // data = spnd.affine_transform(from_img.getData<double>(), deform, trans, to_img.dim(), order=order, mode=mode, cval=cval)

    /*
        RNifti::NiftiImage resampled_img = RNifti::NiftiImage(to_img);
        resampled_img.replaceData(resampled_data, from_img.data().datatype());
    */

    return EXIT_SUCCESS;
}