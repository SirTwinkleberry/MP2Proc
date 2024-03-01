#pragma once

#include <vector>
#include <Eigen/Dense>
#include <boost/timer/timer.hpp>
#include <omp.h>

#include "externals/include/RNifti.h"

#include "src/interpolate.h"
#include "src/mp2rage.h"


template <typename D, typename XT, typename YT, typename ZT>
static ZT COMPUTE_QT1MAP_IN_UNIT(const _2D::LinearDelaunayTriangleInterpolator<D> &interpolator, const XT &B1Map_relative, const YT &T1W_UNIMap_centered, int nThreads = 1, bool verbose = true)
{
    assert(B1Map_relative.size() == T1W_UNIMap_centered.size());

    ZT QT1Map_in_unit(B1Map_relative.size());

    boost::timer::auto_cpu_timer timer;

    if (verbose) {
        std::cout << "===================================================" << "\n" 
                  << "===== Computing qT1 map in provided time unit =====" << "\n"
                  << "===================================================" << "\n"
                  << "Using " << nThreads << " thread(s)"
                  << std::endl;
    }
    
    #pragma omp parallel for num_threads(nThreads) schedule(guided) shared(interpolator, B1Map_relative, T1W_UNIMap_centered, QT1Map_in_unit)
    for (size_t i = 0 ; i < B1Map_relative.size() ; ++i)
        QT1Map_in_unit(i) = interpolator(B1Map_relative(i), T1W_UNIMap_centered(i));

    return (ZT) QT1Map_in_unit;
}

template <typename XT, typename YT>
static YT COMPUTE_QR1MAP_IN_PER_UNIT(const XT &QT1Map_in_unit, bool verbose = true)
{
    boost::timer::auto_cpu_timer timer;

    if (verbose)
    {
        std::cout << "=======================================================" << "\n"
                  << "===== Computing qR1 map in provided per time unit =====" << "\n"
                  << "======================================================="
                  <<  std::endl;
    }

    return (YT) (1. / QT1Map_in_unit.array());
}

template <typename D, typename XT, typename YT>
static _2D::LinearDelaunayTriangleInterpolator<D> INIT_INTERPOLATOR_IN_UNIT(const XT &B1VectorRange_relative, const YT &QT1VectorRange_in_unit, double tInversion1_in_unit, double tInversion2_in_unit, double TRmp2rage_in_unit, double tEchoSpacing_in_unit, int nBefore, int nAfter, double FA1_in_degrees, double FA2_in_degrees, double inversionEfficiency, double M0, bool verbose = true)
{
    boost::timer::auto_cpu_timer timer;

    if (verbose)
    {
        std::cout << "======================================================" << "\n"
                  << "===== Initializing Linear Scattered Interpolator =====" << "\n"
                  << "======================================================"
                  <<  std::endl;
    }

    auto UNIVectorRange_centered = MP2RAGE<const XT, const YT, Eigen::MatrixXd>(B1VectorRange_relative, QT1VectorRange_in_unit
    , tInversion1_in_unit, tInversion2_in_unit, TRmp2rage_in_unit, tEchoSpacing_in_unit
    , nBefore, nAfter, FA1_in_degrees, FA2_in_degrees
    , inversionEfficiency, M0, verbose);

    auto UNIVectorRange_centered_bijectivity_restored = RESTORE_BIJECTIVITY<const Eigen::MatrixXd, const YT>(UNIVectorRange_centered, QT1VectorRange_in_unit, verbose);

    return INTERPOLATOR<D, const XT, const Eigen::MatrixXd, const YT>(
        (B1VectorRange_relative.matrix() * XT::Ones(QT1VectorRange_in_unit.size()).matrix().transpose()).reshaped()
        , UNIVectorRange_centered_bijectivity_restored.matrix().reshaped()
        , (YT::Ones(B1VectorRange_relative.size()).matrix() * QT1VectorRange_in_unit.matrix().transpose()).reshaped()
        , verbose);
}

template <typename XT, typename YT>
static YT COMPUTE_BACK_B1CORRECTED_T1W_UNIMAP_CENTERED(const XT &QT1Map_in_unit, double tInversion1_in_unit, double tInversion2_in_unit, double TRmp2rage_in_unit, double tEchoSpacing_in_unit, int nBefore, int nAfter, double FA1_in_degrees, double FA2_in_degrees, double inversionEfficiency, double M0, int nThreads = 1, bool verbose = true)
{
    boost::timer::auto_cpu_timer timer;

    if (verbose)
    {
        std::cout << "==============================================" << "\n"
                  << "=====   Computing T1-weighted UNI map    =====" << "\n"
                  << "===== with back propagated B1 correction =====" << "\n"
                  << "=============================================="
                  <<  std::endl;
    }

    return MP2RAGE_B1REL_UNITY<const XT, YT>(
        QT1Map_in_unit
        , tInversion1_in_unit
        , tInversion2_in_unit
        , TRmp2rage_in_unit
        , tEchoSpacing_in_unit
        , nBefore
        , nAfter
        , FA1_in_degrees
        , FA2_in_degrees
        , inversionEfficiency
        , M0
        , nThreads
        , verbose);
}

template <typename XT, typename YT>
static XT MASK_FROM_REFERENCE(const XT &ARRAY_TO_MASK, const YT &REFERENCE, bool verbose = true)
{
    assert(ARRAY_TO_MASK.size() == REFERENCE.size());

    boost::timer::auto_cpu_timer timer;

    if (verbose)
    {
        std::cout << "=======================================================" << "\n"
                  << "===== Computing masked map from reference's zeros =====" << "\n"
                  << "======================================================="
                  <<  std::endl;
    }

    XT out = XT::Zero(ARRAY_TO_MASK.size());

    for (size_t i = 0 ; i < ARRAY_TO_MASK.size() ; ++i)
        out(i) = REFERENCE(i) == 0 ? 0 : ARRAY_TO_MASK(i);
    
    return out;
}

template <typename XT, typename YT, typename ZT>
static ZT MINIMUM_INTENSITY_PROJECTION(const XT &X, const YT &Y, bool verbose = true)
{
    assert(X.size() == Y.size());

    boost::timer::auto_cpu_timer timer;

    if (verbose)
    {
        std::cout << "======================================================" << "\n"
                  << "===== Computing minimum intensity projection map =====" << "\n"
                  << "======================================================"
                  <<  std::endl;
    }

    return (ZT) X.array().min(Y.array());
}

template <typename XT, typename YT>
static YT EDGE_CENTERED(const XT &QT1Map_in_ms, double tInversion1_in_ms = 820., double tInversion2_in_ms = 1320., double TRmp2rage_in_ms = 8000., double tEchoSpacing_in_ms = 2.5, int nBefore = 64, int nAfter = 128, double FA1_in_degrees = 5., double FA2_in_degrees = 5., double inversionEfficiency = 1., double M0 = 1., int nThreads = 1, bool verbose = true)
{
    boost::timer::auto_cpu_timer timer;

    if (verbose)
    {
        std::cout << "=============================================" << "\n"
                  << "=====   Computing Synthetic EDGE map    =====" << "\n"
                  << "=====   from A. MASSIRE et al. (2021)   =====" << "\n"
                  << "===== DOI: 10.1097/RLI.0000000000000718 =====" << "\n"
                  << "============================================="
                  <<  std::endl;
    }

    return MP2RAGE_B1REL_UNITY<const XT, YT>(
        QT1Map_in_ms
        , tInversion1_in_ms
        , tInversion2_in_ms
        , TRmp2rage_in_ms
        , tEchoSpacing_in_ms
        , nBefore
        , nAfter
        , FA1_in_degrees
        , FA2_in_degrees
        , inversionEfficiency
        , M0
        , nThreads
        , verbose);
}


template <typename XT, typename YT>
static YT FLAWS_CENTERED(const XT &QT1Map_in_ms, double FLAWS1_tInversion1_in_ms = 900., double FLAWS1_tInversion2_in_ms = 3700., double FLAWS1_TRmp2rage_in_ms = 8250., double FLAWS1_tEchoSpacing_in_ms = 7.5, int FLAWS1_nBefore = 64, int FLAWS1_nAfter = 128, double FLAWS1_FA1_in_degrees = 9., double FLAWS1_FA2_in_degrees = 5., double FLAWS1_inversionEfficiency = 1., double FLAWS1_M0 = 1., double FLAWS2_tInversion1_in_ms = 200., double FLAWS2_tInversion2_in_ms = 1200., double FLAWS2_TRmp2rage_in_ms = 5000., double FLAWS2_tEchoSpacing_in_ms = 3.1, int FLAWS2_nBefore = 64, int FLAWS2_nAfter = 128, double FLAWS2_FA1_in_degrees = 5., double FLAWS2_FA2_in_degrees = 5., double FLAWS2_inversionEfficiency = 1., double FLAWS2_M0 = 1., int nThreads = 1, bool verbose = true)
{
    boost::timer::auto_cpu_timer timer;

    if (verbose)
    {
        std::cout << "=============================================" << "\n"
                  << "=====   Computing Synthetic FLAWS map   =====" << "\n"
                  << "=====   from A. MASSIRE et al. (2021)   =====" << "\n"
                  << "===== DOI: 10.1097/RLI.0000000000000718 =====" << "\n"
                  << "============================================="
                  <<  std::endl;
    }

    auto flaws1 = MP2RAGE_B1REL_UNITY<const XT, YT>(
        QT1Map_in_ms
        , FLAWS1_tInversion1_in_ms
        , FLAWS1_tInversion2_in_ms
        , FLAWS1_TRmp2rage_in_ms
        , FLAWS1_tEchoSpacing_in_ms
        , FLAWS1_nBefore
        , FLAWS1_nAfter
        , FLAWS1_FA1_in_degrees
        , FLAWS1_FA2_in_degrees
        , FLAWS1_inversionEfficiency
        , FLAWS1_M0
        , nThreads
        , verbose);

    auto flaws2 = MP2RAGE_B1REL_UNITY<const XT, YT>(
        QT1Map_in_ms
        , FLAWS2_tInversion1_in_ms
        , FLAWS2_tInversion2_in_ms
        , FLAWS2_TRmp2rage_in_ms
        , FLAWS2_tEchoSpacing_in_ms
        , FLAWS2_nBefore
        , FLAWS2_nAfter
        , FLAWS2_FA1_in_degrees
        , FLAWS2_FA2_in_degrees
        , FLAWS2_inversionEfficiency
        , FLAWS2_M0
        , nThreads
        , verbose);

    return MINIMUM_INTENSITY_PROJECTION<YT, YT, YT>(flaws1, flaws2, verbose);
}


/**
 * @brief 
 * 
 * @tparam D 
 * @tparam T 
 * @param path 
 * @param data 
 * @param reference 
 * @param datatype 
 * @param do_round 
 * @param verbose 
 * @return true 
 * @return false 
 */
template <typename D, typename T>
static bool DATA_TO_FILE(const std::string &path, const T &data, const RNifti::NiftiImage &reference, int datatype, bool do_round = false, bool verbose = true)
{
    try 
    {
        if ( verbose )
            std::cout << "EXPORTING to " << path << std::endl;
        
        std::vector<D> vec = std::vector<D>(data.data(), data.data() + data.size()); 
        
        if ( do_round )
            std::transform(vec.begin(), vec.end(), vec.begin(), [](auto& c){return round(c);});

        RNifti::NiftiImage image = RNifti::NiftiImage(reference);
        image.replaceData(vec, datatype);
        image.toFile(path);

        return true;
    }
    catch (const std::exception &e)
    {
        if ( verbose )
            std::cout << "Failed to save to: " << path << "\n" << e.what() << std::endl;
        return false;
    }
}


template <typename T>
static void EXPORT_RESULTS(const std::vector<std::pair<std::string, T*>> &data_vector, const RNifti::NiftiImage &reference, int datatype = 512, bool do_round = true, int n_threads = 1, bool verbose = true)
{
    boost::timer::auto_cpu_timer timer;

    if ( verbose )
    {
        std::cout << "=================================" << "\n"
                  << "=====   EXPORTING RESULTS   =====" << "\n"
                  << "=================================" << "\n"
                  << "Using " << n_threads << " thread(s)"
                  <<  std::endl;
    }

    #pragma omp parallel for num_threads(n_threads) schedule(static, 1) shared(reference, datatype, do_round, verbose)
    for ( auto it = data_vector.begin() ; it < data_vector.end() ; ++it )
    {
            DATA_TO_FILE<double, T>(
                    (*it).first
                    , *(*it).second
                    , reference
                    , datatype
                    , do_round
                    , verbose
                );
        }
}


// static void SMOOTH() {}

// static void RESAMPLE_AND_ROTATE() {}


/*
    OVERLOADS
*/

/**
 * @brief Overload of `EXPORT_RESULTS<T>(...)`
 */
static void EXPORT_RESULTS(const std::vector<std::pair<std::string, Eigen::ArrayXd*>> &data_vector, const RNifti::NiftiImage &reference, int datatype = 512, bool do_round = true, int n_threads = 1, bool verbose = true)
{
    EXPORT_RESULTS<Eigen::ArrayXd>(data_vector, reference, datatype, do_round, n_threads, verbose);
}


/**
 * @brief Overload of `DATA_TO_FILE<D, T>(...)`
 */
static bool DATA_TO_FILE(const std::string &path, const Eigen::ArrayXd &data, const RNifti::NiftiImage &reference, int datatype, bool do_round = false, bool verbose = true)
{
    return DATA_TO_FILE<double, Eigen::ArrayXd>(path, data, reference, datatype, do_round, verbose);
}


/**
 * @brief Overload of `COMPUTE_QT1MAP_IN_UNIT<D, XT, YT, ZT>(...)`
 */
template <typename D, typename T>
static T COMPUTE_QT1MAP_IN_UNIT(const _2D::LinearDelaunayTriangleInterpolator<D> &interpolator, const T &B1Map_relative, const T &T1W_UNIMap_centered, int nThreads = 1, bool verbose = true)
{
    return COMPUTE_QT1MAP_IN_UNIT<D, T, T, T>(interpolator, B1Map_relative, T1W_UNIMap_centered, nThreads, verbose);
}

/**
 * @brief Overload of `COMPUTE_QT1MAP_IN_UNIT<D, XT, YT, ZT>(...)`
 */
static Eigen::ArrayXd COMPUTE_QT1MAP_IN_UNIT(const _2D::LinearDelaunayTriangleInterpolator<double> &interpolator, const Eigen::ArrayXd &B1Map_relative, const Eigen::ArrayXd &T1W_UNIMap_centered, int nThreads = 1, bool verbose = true)
{
    return COMPUTE_QT1MAP_IN_UNIT<double, Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(interpolator, B1Map_relative, T1W_UNIMap_centered, nThreads, verbose);
}

/**
 * @brief Overload of `COMPUTE_QR1MAP_IN_PER_UNIT<XT, YT>(...)`
 */
template <typename T>
static T COMPUTE_QR1MAP_IN_PER_UNIT(const T &QT1Map_in_unit, bool verbose = true)
{
    return COMPUTE_QR1MAP_IN_PER_UNIT<T, T>(QT1Map_in_unit, verbose);
}

/**
 * @brief Overload of `COMPUTE_QR1MAP_IN_PER_UNIT<XT, YT>(...)`
 */
static Eigen::ArrayXd COMPUTE_QR1MAP_IN_PER_UNIT(const Eigen::ArrayXd &QT1Map_in_unit, bool verbose = true)
{
    return COMPUTE_QR1MAP_IN_PER_UNIT<Eigen::ArrayXd, Eigen::ArrayXd>(QT1Map_in_unit, verbose);
}

/**
 * @brief Overload of `INIT_INTERPOLATOR_IN_UNIT<D, XT, YT>(...)`
 */
template <typename D, typename T>
static _2D::LinearDelaunayTriangleInterpolator<D> INIT_INTERPOLATOR_IN_UNIT(const T &B1VectorRange_relative, const T &QT1VectorRange_in_unit, double tInversion1_in_unit, double tInversion2_in_unit, double TRmp2rage_in_unit, double tEchoSpacing_in_unit, int nBefore, int nAfter, double FA1_in_degrees, double FA2_in_degrees, double inversionEfficiency, double M0, bool verbose = true)
{
    return INIT_INTERPOLATOR_IN_UNIT<D, T, T>(B1VectorRange_relative, QT1VectorRange_in_unit, tInversion1_in_unit, tInversion2_in_unit, TRmp2rage_in_unit, tEchoSpacing_in_unit, nBefore, nAfter, FA1_in_degrees, FA2_in_degrees, inversionEfficiency, M0, verbose);
}

/**
 * @brief Overload of `INIT_INTERPOLATOR_IN_UNIT<D, XT, YT>(...)`
 */
static _2D::LinearDelaunayTriangleInterpolator<double> INIT_INTERPOLATOR_IN_UNIT(const Eigen::ArrayXd &B1VectorRange_relative, const Eigen::ArrayXd &QT1VectorRange_in_unit, double tInversion1_in_unit, double tInversion2_in_unit, double TRmp2rage_in_unit, double tEchoSpacing_in_unit, int nBefore, int nAfter, double FA1_in_degrees, double FA2_in_degrees, double inversionEfficiency, double M0, bool verbose = true)
{
    return INIT_INTERPOLATOR_IN_UNIT<double, Eigen::ArrayXd, Eigen::ArrayXd>(B1VectorRange_relative, QT1VectorRange_in_unit, tInversion1_in_unit, tInversion2_in_unit, TRmp2rage_in_unit, tEchoSpacing_in_unit, nBefore, nAfter, FA1_in_degrees, FA2_in_degrees, inversionEfficiency, M0, verbose);
}

/**
 * @brief Overload of `COMPUTE_BACK_B1CORRECTED_T1W_UNIMAP_CENTERED<XT, YT>(...)`
 */
template <typename T>
static T COMPUTE_BACK_B1CORRECTED_T1W_UNIMAP_CENTERED(const T &QT1Map_in_unit, double tInversion1_in_unit, double tInversion2_in_unit, double TRmp2rage_in_unit, double tEchoSpacing_in_unit, int nBefore, int nAfter, double FA1_in_degrees, double FA2_in_degrees, double inversionEfficiency, double M0, int nThreads = 1, bool verbose = true)
{
    return COMPUTE_BACK_B1CORRECTED_T1W_UNIMAP_CENTERED<T, T>(QT1Map_in_unit, tInversion1_in_unit, tInversion2_in_unit, TRmp2rage_in_unit, tEchoSpacing_in_unit, nBefore, nAfter, FA1_in_degrees, FA2_in_degrees, inversionEfficiency, M0, nThreads, verbose);
}

/**
 * @brief Overload of `COMPUTE_BACK_B1CORRECTED_T1W_UNIMAP_CENTERED<XT, YT>(...)`
 */
static Eigen::ArrayXd COMPUTE_BACK_B1CORRECTED_T1W_UNIMAP_CENTERED(const Eigen::ArrayXd &QT1Map_in_unit, double tInversion1_in_unit, double tInversion2_in_unit, double TRmp2rage_in_unit, double tEchoSpacing_in_unit, int nBefore, int nAfter, double FA1_in_degrees, double FA2_in_degrees, double inversionEfficiency, double M0, int nThreads = 1, bool verbose = true)
{
    return COMPUTE_BACK_B1CORRECTED_T1W_UNIMAP_CENTERED<Eigen::ArrayXd, Eigen::ArrayXd>(QT1Map_in_unit, tInversion1_in_unit, tInversion2_in_unit, TRmp2rage_in_unit, tEchoSpacing_in_unit, nBefore, nAfter, FA1_in_degrees, FA2_in_degrees, inversionEfficiency, M0, nThreads, verbose);
}

/**
 * @brief Overload of `MASK_FROM_REFERENCE<XT, YT>(...)`
 */
template <typename T>
static T MASK_FROM_REFERENCE(const T &ARRAY_TO_MASK, const T &REFERENCE, bool verbose = true)
{
    return MASK_FROM_REFERENCE<T, T>(ARRAY_TO_MASK, REFERENCE, verbose); 
}

/**
 * @brief Overload of `MASK_FROM_REFERENCE<XT, YT>(...)`
 */
static Eigen::ArrayXd MASK_FROM_REFERENCE(const Eigen::ArrayXd &ARRAY_TO_MASK, const Eigen::ArrayXd &REFERENCE, bool verbose = true)
{
    return MASK_FROM_REFERENCE<Eigen::ArrayXd, Eigen::ArrayXd>(ARRAY_TO_MASK, REFERENCE, verbose); 
}

/**
 * @brief Overload of `MINIMUM_INTENSITY_PROJECTION<XT, YT, ZT>(...)`
 */
template <typename T1, typename T2>
static T2 MINIMUM_INTENSITY_PROJECTION(const T1 &X, const T1 &Y, bool verbose = true)
{
    return MINIMUM_INTENSITY_PROJECTION<T1, T1, T2>(X, Y, verbose);
}

/**
 * @brief Overload of `MINIMUM_INTENSITY_PROJECTION<XT, YT, ZT>(...)`
 */
template <typename T>
static T MINIMUM_INTENSITY_PROJECTION(const T &X, const T &Y, bool verbose = true)
{
    return MINIMUM_INTENSITY_PROJECTION<T, T, T>(X, Y, verbose);
}

/**
 * @brief Overload of `MINIMUM_INTENSITY_PROJECTION<XT, YT, ZT>(...)`
 */
static Eigen::ArrayXd MINIMUM_INTENSITY_PROJECTION(const Eigen::ArrayXd &X, const Eigen::ArrayXd &Y, bool verbose = true)
{
    return MINIMUM_INTENSITY_PROJECTION<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(X, Y, verbose);
}

/**
 * @brief Overload of `EDGE_CENTERED<XT, YT>(...)`
 */
template <typename T>
static T EDGE_CENTERED(const T &QT1Map_in_ms, double tInversion1_in_ms = 820., double tInversion2_in_ms = 1320., double TRmp2rage_in_ms = 8000., double tEchoSpacing_in_ms = 2.5, int nBefore = 64, int nAfter = 128, double FA1_in_degrees = 5., double FA2_in_degrees = 5., double inversionEfficiency = 1., double M0 = 1., int nThreads = 1, bool verbose = true)
{
    return EDGE_CENTERED<T, T>(QT1Map_in_ms, tInversion1_in_ms, tInversion2_in_ms, TRmp2rage_in_ms, tEchoSpacing_in_ms, nBefore, nAfter, FA1_in_degrees, FA2_in_degrees, inversionEfficiency, M0, verbose);
}

/**
 * @brief Overload of `EDGE_CENTERED<XT, YT>(...)`
 */
static Eigen::ArrayXd EDGE_CENTERED(const Eigen::ArrayXd &QT1Map_in_ms, double tInversion1_in_ms = 820., double tInversion2_in_ms = 1320., double TRmp2rage_in_ms = 8000., double tEchoSpacing_in_ms = 2.5, int nBefore = 64, int nAfter = 128, double FA1_in_degrees = 5., double FA2_in_degrees = 5., double inversionEfficiency = 1., double M0 = 1., int nThreads = 1, bool verbose = true)
{
    return EDGE_CENTERED<Eigen::ArrayXd, Eigen::ArrayXd>(QT1Map_in_ms, tInversion1_in_ms, tInversion2_in_ms, TRmp2rage_in_ms, tEchoSpacing_in_ms, nBefore, nAfter, FA1_in_degrees, FA2_in_degrees, inversionEfficiency, M0, verbose);
}

/**
 * @brief Overload of `FLAWS_CENTERED<XT, YT>(...)`
 */
template <typename T>
static T FLAWS_CENTERED(const T &QT1Map_in_ms, double FLAWS1_tInversion1_in_ms = 900., double FLAWS1_tInversion2_in_ms = 3700., double FLAWS1_TRmp2rage_in_ms = 8250., double FLAWS1_tEchoSpacing_in_ms = 7.5, int FLAWS1_nBefore = 64, int FLAWS1_nAfter = 128, double FLAWS1_FA1_in_degrees = 9., double FLAWS1_FA2_in_degrees = 5., double FLAWS1_inversionEfficiency = 1., double FLAWS1_M0 = 1., double FLAWS2_tInversion1_in_ms = 200., double FLAWS2_tInversion2_in_ms = 1200., double FLAWS2_TRmp2rage_in_ms = 5000., double FLAWS2_tEchoSpacing_in_ms = 3.1, int FLAWS2_nBefore = 64, int FLAWS2_nAfter = 128, double FLAWS2_FA1_in_degrees = 5., double FLAWS2_FA2_in_degrees = 5., double FLAWS2_inversionEfficiency = 1., double FLAWS2_M0 = 1., int nThreads = 1, bool verbose = true)
{
    return FLAWS_CENTERED<T, T>(QT1Map_in_ms, FLAWS1_tInversion1_in_ms, FLAWS1_tInversion2_in_ms, FLAWS1_TRmp2rage_in_ms, FLAWS1_tEchoSpacing_in_ms, FLAWS1_nBefore, FLAWS1_nAfter, FLAWS1_FA1_in_degrees, FLAWS1_FA2_in_degrees, FLAWS1_inversionEfficiency, FLAWS1_M0, FLAWS2_tInversion1_in_ms, FLAWS2_tInversion2_in_ms, FLAWS2_TRmp2rage_in_ms, FLAWS2_tEchoSpacing_in_ms, FLAWS2_nBefore, FLAWS2_nAfter, FLAWS2_FA1_in_degrees, FLAWS2_FA2_in_degrees, FLAWS2_inversionEfficiency, FLAWS2_M0, verbose);
}

/**
 * @brief Overload of `FLAWS_CENTERED<XT, YT>(...)`
 */
static Eigen::ArrayXd FLAWS_CENTERED(const Eigen::ArrayXd &QT1Map_in_ms, double FLAWS1_tInversion1_in_ms = 900., double FLAWS1_tInversion2_in_ms = 3700., double FLAWS1_TRmp2rage_in_ms = 8250., double FLAWS1_tEchoSpacing_in_ms = 7.5, int FLAWS1_nBefore = 64, int FLAWS1_nAfter = 128, double FLAWS1_FA1_in_degrees = 9., double FLAWS1_FA2_in_degrees = 5., double FLAWS1_inversionEfficiency = 1., double FLAWS1_M0 = 1., double FLAWS2_tInversion1_in_ms = 200., double FLAWS2_tInversion2_in_ms = 1200., double FLAWS2_TRmp2rage_in_ms = 5000., double FLAWS2_tEchoSpacing_in_ms = 3.1, int FLAWS2_nBefore = 64, int FLAWS2_nAfter = 128, double FLAWS2_FA1_in_degrees = 5., double FLAWS2_FA2_in_degrees = 5., double FLAWS2_inversionEfficiency = 1., double FLAWS2_M0 = 1., int nThreads = 1, bool verbose = true)
{
    return FLAWS_CENTERED<Eigen::ArrayXd, Eigen::ArrayXd>(QT1Map_in_ms, FLAWS1_tInversion1_in_ms, FLAWS1_tInversion2_in_ms, FLAWS1_TRmp2rage_in_ms, FLAWS1_tEchoSpacing_in_ms, FLAWS1_nBefore, FLAWS1_nAfter, FLAWS1_FA1_in_degrees, FLAWS1_FA2_in_degrees, FLAWS1_inversionEfficiency, FLAWS1_M0, FLAWS2_tInversion1_in_ms, FLAWS2_tInversion2_in_ms, FLAWS2_TRmp2rage_in_ms, FLAWS2_tEchoSpacing_in_ms, FLAWS2_nBefore, FLAWS2_nAfter, FLAWS2_FA1_in_degrees, FLAWS2_FA2_in_degrees, FLAWS2_inversionEfficiency, FLAWS2_M0, verbose);
}
