/**
 * @file interpolate.h
 * @author TIMOTHY ANDERSON (SIRTWINKLEBERRY.COM)
 * @brief 
 * @version 1.0
 * @date 2024-03-01
 * 
 * @copyright GPLv3 (c) 2024
 * 
 * @todo check if every STD lib include is necessary
 */

#pragma once

#include <vector>
#include <algorithm>
#include <type_traits>
#include <boost/timer/timer.hpp>

#include "externals/include/Interpolate.hpp"
#include "externals/include/AnyInterpolator.hpp"
#include "externals/include/Eigen/Dense"


/**
 * @brief 
 * 
 * @tparam D 
 * @tparam XT 
 * @tparam YT 
 * @tparam ZT 
 * @param X 
 * @param Y 
 * @param Z 
 * @param verbose 
 * @return _2D::LinearDelaunayTriangleInterpolator<D> 
 */
template <typename D, typename XT, typename YT, typename ZT>
static _2D::LinearDelaunayTriangleInterpolator<D> INTERPOLATOR(const XT &X, const YT &Y, const ZT &Z, bool verbose = true)
{
    assert(X.size() == Y.size());
    assert(X.size() == Z.size());

    boost::timer::auto_cpu_timer timer;

    if ( verbose )
    {
        std::cout << "Creating 2-dimensional scattered interpolant" << "\n"
                  << "using underlying Delaunay triangulation and linear geometric interpolation" << "\n"
                  << "-> No extrapolation: values beyond interpolant range are set to 0" << "\n"
                  << "-> No rescaling of input data" << "\n"
                  << "-> Powered by libinterpolate (https://github.com/CD3/libInterpolate)" << "\n"
                  << "              delaunator-cpp (https://github.com/delfrrr/delaunator-cpp)" << "\n"
                  << "              boost          (https://github.com/boostorg/boost)"
                  << std::endl;
    }

    _2D::LinearDelaunayTriangleInterpolator<D> interpolator;
    interpolator.setData(X, Y, Z);

    return interpolator;
}

/**
 * @brief 
 * 
 * @tparam XT 
 * @tparam YT 
 * @param X 
 * @param BOUNDS 
 * @param verbose 
 * @return XT 
 */
template <typename XT, typename YT>
static XT RESTORE_BIJECTIVITY(const XT &X, const YT &BOUNDS, std::vector<std::pair<double, double>> *bijectivity_range, bool verbose = true)
{
    boost::timer::auto_cpu_timer timer;

    if ( verbose )
    {
        std::cout << "Restoring Bijectivity of 2-dimensional array" << "\n"
                  << "Original bounds  : " << BOUNDS.array().minCoeff() << " < T1 (in provided unit) < " << BOUNDS.array().maxCoeff()
                  << std::endl;
    }

    Eigen::MatrixXd out = X;

    auto min_bounds = BOUNDS.array().minCoeff();
    auto max_bounds = BOUNDS.array().maxCoeff();
   
    std::vector<int> rangeFrom0(X.cols() - 1), rangeFrom1(X.cols() - 1);
    std::generate(rangeFrom0.begin(), rangeFrom0.end(), [n = 0] () mutable { return n++; });
    std::generate(rangeFrom1.begin(), rangeFrom1.end(), [n = 1] () mutable { return n++; });

    auto diff = out(Eigen::all, rangeFrom1) - out(Eigen::all, rangeFrom0);
    auto mask = diff.array() < 0;

    for ( size_t i = 0 ; i < X.rows() ; ++i )
    {
        std::vector<size_t> idx_bijective_qT1;
        for ( size_t j = 0 ; j < mask.row(i).size() ; ++j )
            if (mask.row(i)[j]) idx_bijective_qT1.push_back(j);

        if ( idx_bijective_qT1.size() != 0 )
        {
            size_t min_idx = *std::min_element(idx_bijective_qT1.begin(), idx_bijective_qT1.end());
            size_t max_idx = *std::max_element(idx_bijective_qT1.begin(), idx_bijective_qT1.end()) + 1;

            min_bounds = std::max(min_bounds, BOUNDS.row(min_idx)[0]);
            max_bounds = std::min(max_bounds, BOUNDS.row(max_idx)[0]);
            
            auto min_signal = out.row(i)[min_idx];
            auto max_signal = out.row(i)[max_idx];

            for ( size_t j = 0 ; j < min_idx ; ++j )
                out(i, j) = min_signal;
            for ( size_t j = max_idx ; j < out.cols() ; ++j )
                out(i, j) = max_signal;
        }

        bijectivity_range->push_back(std::pair<double, double>(min_bounds, max_bounds));
    }

    if ( verbose )
    {
        std::cout << "Global bijectivity range: " << min_bounds << " < T1 (in provided unit) < " << max_bounds << std::endl;
        std::cout << "Local bijectivity range:" << std::endl;
        for ( const std::pair<double, double> & pair : *bijectivity_range )
            std::cout << "\t" << pair.first << " < T1 (in provided unit) < " << pair.second << std::endl;
    }
    return (XT) out;
}

/*
    OVERLOADS
*/

/**
 * @brief Overload of `RESTORE_BIJECTIVITY<XT, YT>(...)`
 */
static Eigen::ArrayXd RESTORE_BIJECTIVITY(const Eigen::ArrayXd &X, const Eigen::ArrayXd &BOUNDS, std::vector<std::pair<double, double>> *bijectivity_range, bool verbose = true)
{
    return RESTORE_BIJECTIVITY<Eigen::ArrayXd, Eigen::ArrayXd>(X, BOUNDS, bijectivity_range, verbose);
}

/**
 * @brief Overload of `RESTORE_BIJECTIVITY<XT, YT>(...)`
 */
template <typename T>
static T RESTORE_BIJECTIVITY(const T &X, const T &BOUNDS, std::vector<std::pair<double, double>> *bijectivity_range, bool verbose = true)
{
    return RESTORE_BIJECTIVITY<T, T>(X, BOUNDS, bijectivity_range, verbose);
}

/**
 * @brief Overload of `INTERPOLATOR<D, XT, YT, ZT>(...)`
 */
template <typename D, typename T>
static _2D::LinearDelaunayTriangleInterpolator<D> INTERPOLATOR(const T &X, const T &Y, const T &Z, bool verbose = true)
{
    return INTERPOLATOR<D, T, T, T>(X, Y, Z, verbose);
}

/**
 * @brief Overload of `INTERPOLATOR<D, XT, YT, ZT>(...)`
 */
static _2D::LinearDelaunayTriangleInterpolator<double> INTERPOLATOR(const Eigen::ArrayXd &X, const Eigen::ArrayXd &Y, const Eigen::ArrayXd &Z, bool verbose = true)
{
    return INTERPOLATOR<double, Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(X, Y, Z, verbose);
}
