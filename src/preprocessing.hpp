/**
 * @file preprocessing.h
 * @author TIMOTHY ANDERSON (SIRTWINKLEBERRY.COM)
 * @brief ANTS UTILITIES
 * @version 1.0
 * @date 2024-03-03
 * 
 * @copyright GPLv3 (c) 2024
 * 
 */

#pragma once

#include <vector>       // Necessary for ANTs somehow
#include <string>       // Necessary for ANTs somehow
#include <iostream>     // Necessary for ANTs somehow
#include <boost/timer/timer.hpp>

#include "externals/include/ANTs/antsApplyTransforms.h"
#include "externals/include/ANTs/SmoothImage.h"


/**
 * @brief 
 * 
 * @param input 
 * @param reference 
 * @param output 
 * @param output_datatype 
 * @param transforms 
 * @param interpolation 
 * @param default_value 
 * @param dim 
 * @param input_imgtype 
 * @param verbose 
 * @return int 
 */
static int ANTS_APPLY_TRANSFORMS(const std::string &input, const std::string &reference, const std::string &output, const std::string &output_datatype = "default", const std::string &transforms = "identity", const std::string &interpolation = "BSpline[3]", double default_value = 0, ushort dim = 3, ushort input_imgtype = 0, bool verbose = true)
{
    boost::timer::auto_cpu_timer timer;

    std::vector<std::string> TRANSFORMS_VECTOR;
    
    if (verbose)
    {
        std::cout << "=======================================================" << "\n" 
                  << "===== \033[1;35mComputing transformed volume from reference\033[0m =====" << "\n"
                  << "=======================================================" << "\n"
                  << "Using ANTs software   `\033[1;33mantsApplyTransforms\033[0m`   with:"     << "\n"
                  << "\033[36mNumber of dimensions       \033[0m: " << dim                    << "\n"
                  << "\033[96mANTs volume type code      \033[0m: " << input_imgtype          << "\n"
                  << "\033[36mOutput datatype            \033[0m: " << output_datatype        << "\n"
                  << "\033[96mTransforms array           \033[0m: " << transforms             << "\n"
                  << "\033[36mInterpolation method       \033[0m: " << interpolation          << "\n"
                  << "\033[96mDefault extrapolation value\033[0m: " << default_value          << "\n"
                  << "\033[36mInput volume               \033[0m: " << input                  << "\n"
                  << "\033[96mReference volume           \033[0m: " << reference              << "\n"
                  << "\033[36mOutput volume              \033[0m: " << output
                  << std::endl;

        TRANSFORMS_VECTOR.push_back("-v");
        TRANSFORMS_VECTOR.push_back("1");
    }

    TRANSFORMS_VECTOR.push_back("-d");
    TRANSFORMS_VECTOR.push_back(std::to_string(dim));
    TRANSFORMS_VECTOR.push_back("-e");
    TRANSFORMS_VECTOR.push_back(std::to_string(input_imgtype));
    TRANSFORMS_VECTOR.push_back("-u");
    TRANSFORMS_VECTOR.push_back(output_datatype);
    TRANSFORMS_VECTOR.push_back("-t");
    TRANSFORMS_VECTOR.push_back(transforms);
    TRANSFORMS_VECTOR.push_back("-n");
    TRANSFORMS_VECTOR.push_back(interpolation);
    TRANSFORMS_VECTOR.push_back("-f");
    TRANSFORMS_VECTOR.push_back(std::to_string(default_value));
    TRANSFORMS_VECTOR.push_back("-i");
    TRANSFORMS_VECTOR.push_back(input);
    TRANSFORMS_VECTOR.push_back("-r");
    TRANSFORMS_VECTOR.push_back(reference);
    TRANSFORMS_VECTOR.push_back("-o");
    TRANSFORMS_VECTOR.push_back(output);

    return ants::antsApplyTransforms(TRANSFORMS_VECTOR, &std::cout);
}

/**
 * @brief 
 * 
 * @param input 
 * @param output 
 * @param sigma 
 * @param is_sigma_in_spacing_units 
 * @param use_median_filtering 
 * @param verbose 
 * @return int 
 */
static int ANTS_SMOOTH_IMAGE(const std::string &input, const std::string &output, ushort dim = 3, std::string sigma = "1x1x1", bool is_sigma_in_spacing_units = false, bool use_median_filtering = false, bool verbose = true)
{
    boost::timer::auto_cpu_timer timer;

    if (verbose) {
        std::cout << "=============================================="        << "\n" 
                  << "===== \033[1;35mApplying Gaussian smooth to volume\033[0m ====="        << "\n"
                  << "=============================================="        << "\n"
                  << "Using ANTs software   `\033[1;33mSmoothImage\033[0m`   with:"           << "\n"
                  << "\033[36mNumber of dimensions  \033[0m: " << dim                       << "\n"
                  << "\033[96mGaussian STD          \033[0m: " << sigma                     << "\n"
                  << "\033[36mIs in spacing units   \033[0m: " << is_sigma_in_spacing_units << "\n"
                  << "\033[96mUsing median filtering\033[0m: " << use_median_filtering      << "\n"
                  << "\033[36mInput volume          \033[0m: " << input                     << "\n"
                  << "\033[96mOutput volume         \033[0m: " << output                    << "\n"
                  << std::endl;
    }

    std::vector<std::string> SMOOTH_VECTOR;

    SMOOTH_VECTOR.push_back(std::to_string(dim));
    SMOOTH_VECTOR.push_back(input);
    SMOOTH_VECTOR.push_back(sigma);
    SMOOTH_VECTOR.push_back(output);
    SMOOTH_VECTOR.push_back(std::to_string((int) is_sigma_in_spacing_units));
    SMOOTH_VECTOR.push_back(std::to_string((int) use_median_filtering));

    return ants::SmoothImage(SMOOTH_VECTOR, &std::cout);
}
