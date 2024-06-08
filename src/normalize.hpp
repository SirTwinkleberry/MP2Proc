/**
 * @file normalize.h
 * @author TIMOTHY ANDERSON (SIRTWINKLEBERRY.COM)
 * @brief 
 * @version 1.0
 * @date 2024-03-01
 * 
 * @copyright GPLv3 (c) 2024
 * 
 */

#pragma once

#include <iostream>
#include <boost/timer/timer.hpp>

#include "externals/include/Eigen/Dense"


/**
 * @brief 
 * 
 * @tparam T1 
 * @tparam T2 
 * @param data 
 * @param shift 
 * @param norm 
 * @param label 
 * @param verbose 
 * @return T2 
 */
template <typename T1, typename T2>
static T2 _NORMALIZE(const T1 &data, double shift, double norm, std::string label = "", bool verbose = true)
{
    boost::timer::auto_cpu_timer timer;
    if (verbose)
    {
        std::cout << "Normalizing " << label << " with shift = " << shift << " and norm = " << norm << std::endl;
    }

    return (T2) (data.array() + shift) * norm;
}

/**
 * @brief 
 * 
 * @tparam T1 
 * @tparam T2 
 * @param data 
 * @param verbose 
 * @return T2 
 */
template <typename T1, typename T2>
static T2 MS_TO_S(const T1 &data, bool verbose = true)
{
    return _NORMALIZE<T1, T2>(data, 0, 1./1000, "to unit [s]", verbose);
}

/**
 * @brief 
 * 
 * @tparam T1 
 * @tparam T2 
 * @param data 
 * @param verbose 
 * @return T2 
 */
template <typename T1, typename T2>
static T2 S_TO_MS(const T1 &data, bool verbose = true)
{
    return _NORMALIZE<T1, T2>(data, 0, 1000, "to unit [ms]", verbose);
}

/**
 * @brief 
 * 
 * @tparam T1 
 * @tparam T2 
 * @param data 
 * @param nominalB1 
 * @param verbose 
 * @return T2 
 */
template <typename T1, typename T2>
static T2 B1_TO_RELATIVE_B1(const T1 &data, double nominalB1, double gainB1 = 1.0, double gainT1 = 1.0, bool verbose = true)
{
    return _NORMALIZE<T1, T2>(data, 0, gainT1 / (gainB1 * nominalB1), "to relative B_1", verbose);
}

/**
 * @brief 
 * 
 * @tparam T1 
 * @tparam T2 
 * @param data 
 * @param nominalB1 
 * @param verbose 
 * @return T2 
 */
template <typename T1, typename T2>
static T2 RELATIVE_B1_TO_B1(const T1 &data, double nominalB1, double gainB1 = 1.0, double gainT1 = 1.0, bool verbose = true)
{
    return _NORMALIZE<T1, T2>(data, 0, gainB1 * nominalB1 / gainT1, "to absolute B_1", verbose);
}

/**
 * @brief 
 * 
 * @tparam T1 
 * @tparam T2 
 * @param data 
 * @param verbose 
 * @return T2 
 */
template <typename T1, typename T2>
static T2 TO_12BITS_RANGE(const T1 &data, bool verbose = true)
{
    return _NORMALIZE<T1, T2>(data, 0.5, 4095, "to [0, 4095] range", verbose);
}

/**
 * @brief 
 * 
 * @tparam T1 
 * @tparam T2 
 * @param data 
 * @param verbose 
 * @return T2 
 */
template <typename T1, typename T2>
static T2 TO_UNI_RANGE(const T1 &data, bool verbose = true)
{
    return _NORMALIZE<T1, T2>(data, -2047.5, 1./4095, "to [-.5, .5] range", verbose);
}

/**
 * @brief 
 * 
 * @tparam T1 
 * @tparam T2 
 * @param data 
 * @param verbose 
 * @return T2 
 * 
 * @deprecated  As of release 1.0, replaced by {@link #TO_12BITS_RANGE(const T1 &data, bool verbose)}
 */
template <typename T1, typename T2>
[[deprecated("\033[1;33mUse `T2 TO_12BITS_RANGE(const T1 &data, bool verbose)` instead.\033[0m")]]
static T2 TO_12BITS_RANGE_DEPRECATED(const T1 &data, bool verbose = true)
{
    return _NORMALIZE<T1, T2>(data, 0.5, 4096, "to [0, 4096] range /!\\ [DEPRECATED]", verbose);
}

/**
 * @brief 
 * 
 * @tparam T1 
 * @tparam T2 
 * @param data 
 * @param verbose 
 * @return T2 
 * 
 * @deprecated  As of release 1.0, replaced by {@link #TO_UNI_RANGE(const T1 &data, bool verbose)}
 */
template <typename T1, typename T2>
[[deprecated("\033[1;33mUse `T2 TO_UNI_RANGE(const T1 &data, bool verbose)` instead.\033[0m")]]
static T2 TO_UNI_RANGE_DEPRECATED(const T1 &data, bool verbose = true)
{
    return _NORMALIZE<T1, T2>(data, -2048, 1./4096, "to [-.5, .0.499755859375] range /!\\ [DEPRECATED]", verbose);
}

/*
    OVERLOADS
*/

/**
 * @brief Overload of `_NORMALIZE<T1, T2>(...)`
 */
template <typename T1>
static T1 _NORMALIZE(const T1 &data, double shift, double norm, std::string label = "", bool verbose = true)
{
    return _NORMALIZE<T1, T1>(data, shift, norm, label, verbose);
}

/**
 * @brief Overload of `_NORMALIZE<T1, T2>(...)`
 */
static Eigen::ArrayXd _NORMALIZE(const Eigen::ArrayXd &data, double shift, double norm, std::string label = "", bool verbose = true)
{
    return _NORMALIZE<Eigen::ArrayXd, Eigen::ArrayXd>(data, shift, norm, label, verbose);
}

/**
 * @brief Overload of `TO_UNI_RANGE_DEPRECATED<T1, T2>(...)`
 */
template <typename T1>
static T1 TO_UNI_RANGE_DEPRECATED(const T1 &data, bool verbose = true)
{
    return TO_UNI_RANGE_DEPRECATED<T1, T1>(data, verbose);
}

/**
 * @brief Overload of `TO_UNI_RANGE_DEPRECATED<T1, T2>(...)`
 */
static Eigen::ArrayXd TO_UNI_RANGE_DEPRECATED(const Eigen::ArrayXd &data, bool verbose = true)
{
    return TO_UNI_RANGE_DEPRECATED<Eigen::ArrayXd, Eigen::ArrayXd>(data, verbose);
}

/**
 * @brief Overload of `TO_12BITS_RANGE_DEPRECATED<T1, T2>(...)`
 */
template <typename T1>
static T1 TO_12BITS_RANGE_DEPRECATED(const T1 &data, bool verbose = true)
{
    return TO_12BITS_RANGE_DEPRECATED<T1, T1>(data, verbose);
}

/**
 * @brief Overload of `TO_12BITS_RANGE_DEPRECATED<T1, T2>(...)`
 */
static Eigen::ArrayXd TO_12BITS_RANGE_DEPRECATED(const Eigen::ArrayXd &data, bool verbose = true)
{
    return TO_12BITS_RANGE_DEPRECATED<Eigen::ArrayXd, Eigen::ArrayXd>(data, verbose);
}

/**
 * @brief Overload of `TO_UNI_RANGE<T1, T2>(...)`
 */
template <typename T1>
static T1 TO_UNI_RANGE(const T1 &data, bool verbose = true)
{
    return TO_UNI_RANGE<T1, T1>(data, verbose);
}

/**
 * @brief Overload of `TO_UNI_RANGE<T1, T2>(...)`
 */
static Eigen::ArrayXd TO_UNI_RANGE(const Eigen::ArrayXd &data, bool verbose = true)
{
    return TO_UNI_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(data, verbose);
}

/**
 * @brief Overload of `TO_12BITS_RANGE<T1, T2>(...)`
 */
template <typename T1>
static T1 TO_12BITS_RANGE(const T1 &data, bool verbose = true)
{
    return TO_12BITS_RANGE<T1, T1>(data, verbose);
}

/**
 * @brief Overload of `TO_12BITS_RANGE<T1, T2>(...)`
 */
static Eigen::ArrayXd TO_12BITS_RANGE(const Eigen::ArrayXd &data, bool verbose = true)
{
    return TO_12BITS_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(data, verbose);
}

/**
 * @brief Overload of `RELATIVE_B1_TO_B1<T1, T2>(...)`
 */
template <typename T1>
static T1 RELATIVE_B1_TO_B1(const T1 &data, double nominalB1, double gainB1 = 1.0, double gainT1 = 1.0, bool verbose = true)
{
    return RELATIVE_B1_TO_B1<T1, T1>(data, gainB1, nominalB1, gainB1, gainT1, gainT1, verbose);
}

/**
 * @brief Overload of `RELATIVE_B1_TO_B1<T1, T2>(...)`
 */
static Eigen::ArrayXd RELATIVE_B1_TO_B1(const Eigen::ArrayXd &data, double nominalB1, double gainB1 = 1.0, double gainT1 = 1.0, bool verbose = true)
{
    return RELATIVE_B1_TO_B1<Eigen::ArrayXd, Eigen::ArrayXd>(data, nominalB1, gainB1, gainT1, verbose);
}

/**
 * @brief Overload of `B1_TO_RELATIVE_B1<T1, T2>(...)`
 */
template <typename T1>
static T1 B1_TO_RELATIVE_B1(const T1 &data, double nominalB1, double gainB1 = 1.0, double gainT1 = 1.0, bool verbose = true)
{
    return B1_TO_RELATIVE_B1<T1, T1>(data, nominalB1, gainB1, gainT1, verbose);
}

/**
 * @brief Overload of `B1_TO_RELATIVE_B1<T1, T2>(...)`
 */
static Eigen::ArrayXd B1_TO_RELATIVE_B1(const Eigen::ArrayXd &data, double nominalB1, double gainB1 = 1.0, double gainT1 = 1.0, bool verbose = true)
{
    return B1_TO_RELATIVE_B1<Eigen::ArrayXd, Eigen::ArrayXd>(data, nominalB1, gainB1, gainT1, verbose);
}

/**
 * @brief Overload of `S_TO_MS<T1, T2>(...)`
 */
template <typename T1>
static T1 S_TO_MS(const T1 &data, bool verbose = true)
{
    return S_TO_MS<T1, T1>(data, verbose);
}

/**
 * @brief Overload of `S_TO_MS<T1, T2>(...)`
 */
static Eigen::ArrayXd S_TO_MS(const Eigen::ArrayXd &data, bool verbose = true)
{
    return S_TO_MS<Eigen::ArrayXd, Eigen::ArrayXd>(data, verbose);
}

/**
 * @brief Overload of `MS_TO_S<T1, T2>(...)`
 */
template <typename T1>
static T1 MS_TO_S(const T1 &data, bool verbose = true)
{
    return MS_TO_S<T1, T1>(data, verbose);
}

/**
 * @brief Overload of `MS_TO_S<T1, T2>(...)`
 */
static Eigen::ArrayXd MS_TO_S(const Eigen::ArrayXd &data, bool verbose = true)
{
    return MS_TO_S<Eigen::ArrayXd, Eigen::ArrayXd>(data, verbose);
}
