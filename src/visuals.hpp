/**
 * @file visuals.h
 * @author TIMOTHY ANDERSON (SIRTWINKLEBERRY.COM)
 * @brief 
 * @version 1.0
 * @date 2024-06-09
 * 
 * @copyright GPLv3 (c) 2024
 * 
 */

#pragma once

#include <vector>
#include <boost/timer/timer.hpp>

#include "externals/include/Eigen/Dense"
#include "externals/include/matplotlibcpp.h"


/**
 * @brief 
 * 
 * @tparam XT 
 * @tparam YT 
 * @tparam ZT 
 * @param B1VectorRange_relative 
 * @param QT1VectorRange_in_unit 
 * @param UNIVectorRange_centered 
 * @param base_path 
 * @param show 
 * @param to_file 
 * @param verbose 
 * @return true 
 * @return false 
 */
template <typename XT, typename YT, typename ZT>
bool PLOT_INTERPOLATION_HYPERSURFACE(const XT &B1VectorRange_relative, const YT &QT1VectorRange_in_unit, const ZT &UNIVectorRange_centered, const std::string &base_path, bool show, bool to_file, bool verbose)
{
    boost::timer::auto_cpu_timer timer;

    if (verbose)
    {
        std::cout << "======================================================" << "\n"
                  << "========= \033[1;35mPlotting Interpolant Hypersurface\033[0m ==========" << "\n"
                  << "======================================================"
                  <<  std::endl;
    }

    Eigen::ArrayXd x = B1VectorRange_relative.reshaped();
    Eigen::ArrayXd y = QT1VectorRange_in_unit.reshaped();
    Eigen::ArrayXd z = UNIVectorRange_centered.reshaped();

    size_t sizeX = x.size();
    size_t sizeY = y.size();
    size_t sizeZ = z.size();

    assert(sizeX * sizeY == sizeZ);

    std::vector<std::vector<double>> X;
    std::vector<std::vector<double>> Y;
    std::vector<std::vector<double>> Z;
    auto it = z.begin();

    for ( size_t i = 0 ; i < sizeY ; ++i )
    {
        X.push_back(std::vector<double>(x.begin(), x.end()));
        Y.push_back(std::vector<double>(sizeX, y(i)));
        Z.push_back(std::vector<double>(it, it + sizeX));
        it += sizeX;
    }

    // matplotlibcpp::plot_surface(X, Y, Z, std::map<std::string, std::string>{{"shade", "False"}, {"alpha",".5"}, {"rstride","25"}, {"cstride","25"}});
    matplotlibcpp::plot_wireframe(X, Y, Z, std::map<std::string, std::string>{{"antialiased","True"}, {"alpha",".5"}, {"rstride","100"}, {"cstride","25"}});
    matplotlibcpp::set_proj_type("ortho");
    if ( to_file ) matplotlibcpp::save(base_path + ".png");
    if ( show ) matplotlibcpp::show();

    matplotlibcpp::detail::_interpreter::kill();

    return true;
}


/*
    OVERLOADS
*/

/**
 * @brief Overload of `PLOT_INTERPOLATION_HYPERSURFACE<XT, YT, ZT>(...)`
 */
template <typename T>
bool PLOT_INTERPOLATION_HYPERSURFACE(const T &B1VectorRange_relative, const T &QT1VectorRange_in_unit, const T &UNIVectorRange_centered, const std::string &base_path, bool show, bool to_file, bool verbose)
{
    return PLOT_INTERPOLATION_HYPERSURFACE<T, T, T>(B1VectorRange_relative, QT1VectorRange_in_unit, UNIVectorRange_centered, base_path, show, to_file, verbose);
}

/**
 * @brief Overload of `PLOT_INTERPOLATION_HYPERSURFACE<XT, YT, ZT>(...)`
 */
bool PLOT_INTERPOLATION_HYPERSURFACE(const Eigen::ArrayXd &B1VectorRange_relative, const Eigen::ArrayXd &QT1VectorRange_in_unit, const Eigen::ArrayXd &UNIVectorRange_centered, const std::string &base_path, bool show, bool to_file, bool verbose)
{
    return PLOT_INTERPOLATION_HYPERSURFACE<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(B1VectorRange_relative, QT1VectorRange_in_unit, UNIVectorRange_centered, base_path, show, to_file, verbose);
}
