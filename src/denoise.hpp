/**
 * @file denoise.h
 * @author TIMOTHY ANDERSON (SIRTWINKLEBERRY.COM)
 * @brief 
 * @version 1.0
 * @date 2024-03-01
 * 
 * @copyright GPLv3 (c) 2024
 * 
 */

#pragma once

#include <boost/timer/timer.hpp>

#include "externals/include/Eigen/Dense"


/**
 * @brief \\frac{\\text{Signal} * (INV1^2 + INV2^2) - \\epsilon^2}{INV1^2 + INV2^2 + 2\\epsilon^2}
 * 
 * @tparam T1 
 * @tparam T2 
 * @tparam T3 
 * @param data_noisy_centered 
 * @param data_at_Inversion1_0_to_4095 
 * @param data_at_Inversion2_0_to_4095 
 * @param epsilon 
 * @param verbose 
 * @return T1 
 */
template <typename T1, typename T2, typename T3>
static T1 DENOISE(const T1 &data_noisy_centered, const T2 &data_at_Inversion1_0_to_4095, const T3 &data_at_Inversion2_0_to_4095, double epsilon = 100, bool verbose = true)
{
    assert(data_noisy_centered.size() == data_at_Inversion1_0_to_4095.size());
    assert(data_noisy_centered.size() == data_at_Inversion2_0_to_4095.size());

    boost::timer::auto_cpu_timer timer;

    if (verbose)
        std::cout << "Standard denoising with epsilon = " << epsilon << " for noisy data in range [-.5, .5]" << std::endl;

    auto SUM_INVSQ = data_at_Inversion1_0_to_4095.array()*data_at_Inversion1_0_to_4095.array() + data_at_Inversion2_0_to_4095.array()*data_at_Inversion2_0_to_4095.array();
    return (T1) ((data_noisy_centered.array() * SUM_INVSQ.array() - epsilon * epsilon) / (2 * epsilon * epsilon + SUM_INVSQ.array()));
}

/** 
 * @brief \\frac{\\text{Signal} * (INV1^2 + INV2^2)}{INV1^2 + INV2^2 + 2\\epsilon^2}
 * 
 * @tparam T1 
 * @tparam T2 
 * @tparam T3 
 * @param data_noisy_0_to_4095 
 * @param data_at_Inversion1_0_to_4095 
 * @param data_at_Inversion2_0_to_4095 
 * @param epsilon 
 * @param verbose 
 * @return T1 
 */
template <typename T1, typename T2, typename T3>
static T1 DENOISE_ALT(const T1 &data_noisy_0_to_4095, const T2 &data_at_Inversion1_0_to_4095, const T3 &data_at_Inversion2_0_to_4095, double epsilon = 100, bool verbose = true)
{
    assert(data_noisy_0_to_4095.size() == data_at_Inversion1_0_to_4095.size());
    assert(data_noisy_0_to_4095.size() == data_at_Inversion2_0_to_4095.size());

    boost::timer::auto_cpu_timer timer;

    if (verbose)
        std::cout << "Alternative denoising with epsilon = " << epsilon << " for noisy data in range [0, 4095]" << std::endl;

    auto SUM_INVSQ = data_at_Inversion1_0_to_4095.array().square() + data_at_Inversion2_0_to_4095.array().square();
    return (T1) ((data_noisy_0_to_4095.array() * SUM_INVSQ.array()) / (2 * epsilon * epsilon + SUM_INVSQ.array()));
}

/*
    OVERLOADS
*/

/**
 * @brief Overload of `DENOISE<T1, T2, T3>(...)`
 */
template <typename T1, typename T2>
static T1 DENOISE(const T1 &data_noisy_centered, const T2 &data_at_Inversion1_0_to_4095, const T2 &data_at_Inversion2_0_to_4095, double epsilon = 100, bool verbose = true)
{
    return DENOISE<T1, T2, T2>(data_noisy_centered, data_at_Inversion1_0_to_4095, data_at_Inversion2_0_to_4095, epsilon, verbose);
}

/**
 * @brief Overload of `DENOISE<T1, T2, T3>(...)`
 */
template <typename T1>
static T1 DENOISE(const T1 &data_noisy_centered, const T1 &data_at_Inversion1_0_to_4095, const T1 &data_at_Inversion2_0_to_4095, double epsilon = 100, bool verbose = true)
{
    return DENOISE<T1, T1, T1>(data_noisy_centered, data_at_Inversion1_0_to_4095, data_at_Inversion2_0_to_4095, epsilon, verbose);
}

/**
 * @brief Overload of `DENOISE<T1, T2, T3>(...)`
 */
static Eigen::ArrayXd DENOISE(const Eigen::ArrayXd &data_noisy_centered, const Eigen::ArrayXd &data_at_Inversion1_0_to_4095, const Eigen::ArrayXd &data_at_Inversion2_0_to_4095, double epsilon = 100, bool verbose = true)
{
    return DENOISE<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(data_noisy_centered, data_at_Inversion1_0_to_4095, data_at_Inversion2_0_to_4095, epsilon, verbose);
}

/**
 * @brief Overload of `DENOISE_ALT<T1, T2, T3>(...)`
 */
template <typename T1, typename T2>
static T1 DENOISE_ALT(const T1 &data_noisy_0_to_4095, const T2 &data_at_Inversion1_0_to_4095, const T2 &data_at_Inversion2_0_to_4095, double epsilon = 100, bool verbose = true)
{
    return DENOISE_ALT<T1, T2, T2>(data_noisy_0_to_4095, data_at_Inversion1_0_to_4095, data_at_Inversion2_0_to_4095, epsilon, verbose);
}

/**
 * @brief Overload of `DENOISE_ALT<T1, T2, T3>(...)`
 */
template <typename T1>
static T1 DENOISE_ALT(const T1 &data_noisy_0_to_4095, const T1 &data_at_Inversion1_0_to_4095, const T1 &data_at_Inversion2_0_to_4095, double epsilon = 100, bool verbose = true)
{
    return DENOISE_ALT<T1, T1, T1>(data_noisy_0_to_4095, data_at_Inversion1_0_to_4095, data_at_Inversion2_0_to_4095, epsilon, verbose);
}

/**
 * @brief Overload of `DENOISE_ALT<T1, T2, T3>(...)`
 */
static Eigen::ArrayXd DENOISE_ALT(const Eigen::ArrayXd &data_noisy_0_to_4095, const Eigen::ArrayXd &data_at_Inversion1_0_to_4095, const Eigen::ArrayXd &data_at_Inversion2_0_to_4095, double epsilon = 100, bool verbose = true)
{
    return DENOISE_ALT<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(data_noisy_0_to_4095, data_at_Inversion1_0_to_4095, data_at_Inversion2_0_to_4095, epsilon, verbose);
}
