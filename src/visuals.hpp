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

#include <fstream>
#include <string>
#include <vector>
#include <boost/timer/timer.hpp>

#include "externals/include/Eigen/Dense"
#include "externals/include/matplotlibcpp.h"


/**
 * @brief 
 * 
 * @tparam T 
 * @param filename 
 * @param dataset 
 */
template <typename T>
void WRITE_CSV(const std::string &filename, const std::vector<std::vector<T>> &dataset)
{
    // Create an output filestream object
    std::ofstream myFile(filename);
        
    size_t sizeX = dataset.size();
    size_t sizeY = dataset.at(0).size();

    // Send data to the stream
    for(size_t i = 0; i < sizeX; ++i)
    {
        for(size_t j = 0; j < sizeY; ++j)
        {
            myFile << dataset.at(i).at(j);
            if(j != sizeY - 1) myFile << ","; // No comma at end of line
        }
        myFile << "\n";
    }
    
    // Close the file
    myFile.close();
}

/**
 * @brief 
 * 
 * @tparam T 
 * @tparam U 
 * @param filename 
 * @param colnames 
 * @param rownames 
 * @param dataset 
 * @param topleft 
 */
template <typename T, typename U>
void WRITE_CSV(const std::string &filename, const std::vector<T> &colnames, const std::vector<T> &rownames, const std::vector<std::vector<U>> &dataset, const std::string &topleft = "")
{
    // Create an output filestream object
    std::ofstream myFile(filename);
        
    size_t sizeX = rownames.size();
    size_t sizeY = colnames.size();
    
    // Header
    myFile << topleft;
    for(size_t i = 0; i < sizeY; ++i)
    {
        myFile << colnames.at(i);
        if(i != sizeY - 1) myFile << ",";
    }
    myFile << "\n";

    // Send data to the stream
    for(size_t i = 0; i < sizeX; ++i)
    {
        // row names
        myFile << rownames.at(i) << ",";

        for(size_t j = 0; j < sizeY; ++j)
        {
            myFile << dataset.at(i).at(j);
            if(j != sizeY - 1) myFile << ","; // No comma at end of line
        }
        myFile << "\n";
    }
    
    // Close the file
    myFile.close();
}

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

    Eigen::ArrayXd x = 100 * B1VectorRange_relative.reshaped();
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
    matplotlibcpp::rcparams(std::pair<double, double>(15, 15));
    matplotlibcpp::plot_wireframe(X, Y, Z, std::map<std::string, std::string>{{"antialiased","True"}, {"alpha",".5"}, {"rstride","100"}, {"cstride","25"}});
    matplotlibcpp::title("$T_1$-w UNI MP2RAGE = $f(B_1,$ $qT_1)$", std::map<std::string, std::string>{{"fontsize", "20"}});
    matplotlibcpp::set_proj_type("ortho");
    matplotlibcpp::xlabel("$B_1$ (%)");
    matplotlibcpp::xlim(0., x.maxCoeff() + 10.);
    matplotlibcpp::ylabel("$qT_1$ (ms)");
    matplotlibcpp::ylim(0., y.maxCoeff() + 100.);
    matplotlibcpp::set_zlabel("$T_1$-w UNI MP2RAGE (a.u.)");
    matplotlibcpp::set_zlim(-0.51, 0.51);

    if ( to_file ) 
    {
        matplotlibcpp::view_init(0, -90, 0);
        matplotlibcpp::save(base_path + "_XZ.png");
        matplotlibcpp::view_init(0, 0, 0);
        matplotlibcpp::save(base_path + "_YZ.png");
        matplotlibcpp::view_init(22.5, 135, 0);
        matplotlibcpp::save(base_path + "_XYZ.png");

        WRITE_CSV<double, double>(base_path + "_data.csv", std::vector<double>(x.begin(), x.end()), std::vector<double>(y.begin(), y.end()), Z, "qT1\\B1,");
    }

    matplotlibcpp::view_init(22.5, 135, 0);
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

/**
 * @brief Overload of `WRITE_CSV<T>(...)`
 */
void WRITE_CSV(const std::string &filename, const std::vector<std::vector<double>> &dataset)
{
    return WRITE_CSV<double>(filename, dataset);
}

/**
 * @brief Overload of `WRITE_CSV<T>(...)`
 */
void WRITE_CSV(const std::string &filename, const std::vector<std::vector<float>> &dataset)
{
    return WRITE_CSV<float>(filename, dataset);
}

/**
 * @brief Overload of `WRITE_CSV<T>(...)`
 */
void WRITE_CSV(const std::string &filename, const std::vector<std::vector<int>> &dataset)
{
    return WRITE_CSV<int>(filename, dataset);
}

/**
 * @brief Overload of `WRITE_CSV<T>(...)`
 */
void WRITE_CSV(const std::string &filename, const std::vector<std::vector<long>> &dataset)
{
    return WRITE_CSV<long>(filename, dataset);
}

/**
 * @brief Overload of `WRITE_CSV<T, U>(...)`
 */
template <typename T>
void WRITE_CSV(const std::string &filename, const std::vector<T> &colnames, const std::vector<T> &rownames, const std::vector<std::vector<T>> &dataset, const std::string &topleft = "")
{
    return WRITE_CSV<T, T>(filename, colnames, rownames, dataset, topleft);
}

/**
 * @brief Overload of `WRITE_CSV<T, U>(...)`
 */
void WRITE_CSV(const std::string &filename, const std::vector<double> &colnames, const std::vector<double> &rownames, const std::vector<std::vector<double>> &dataset, const std::string &topleft = "")
{
    return WRITE_CSV<double, double>(filename, colnames, rownames, dataset, topleft);
}

/**
 * @brief Overload of `WRITE_CSV<T, U>(...)`
 */
void WRITE_CSV(const std::string &filename, const std::vector<float> &colnames, const std::vector<float> &rownames, const std::vector<std::vector<float>> &dataset, const std::string &topleft = "")
{
    return WRITE_CSV<float, float>(filename, colnames, rownames, dataset, topleft);
}

/**
 * @brief Overload of `WRITE_CSV<T, U>(...)`
 */
void WRITE_CSV(const std::string &filename, const std::vector<int> &colnames, const std::vector<int> &rownames, const std::vector<std::vector<int>> &dataset, const std::string &topleft = "")
{
    return WRITE_CSV<int, int>(filename, colnames, rownames, dataset, topleft);
}

/**
 * @brief Overload of `WRITE_CSV<T, U>(...)`
 */
void WRITE_CSV(const std::string &filename, const std::vector<long> &colnames, const std::vector<long> &rownames, const std::vector<std::vector<long>> &dataset, const std::string &topleft = "")
{
    return WRITE_CSV<long, long>(filename, colnames, rownames, dataset, topleft);
}
