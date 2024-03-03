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


#include <vector>       // Necessary for ANTs somehow
#include <string>       // Necessary for ANTs somehow
#include <iostream>     // Necessary for ANTs somehow

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
    std::vector<std::string> TRANSFORMS_VECTOR;
    
    if (verbose)
    {
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
static int ANTS_SMOOTH_IMAGE(const std::string &input, const std::string &output, std::string sigma = "1x1x1", bool is_sigma_in_spacing_units = false, bool use_median_filtering = false, bool verbose = true)
{
    std::vector<std::string> SMOOTH_VECTOR;

    SMOOTH_VECTOR.push_back("3");
    SMOOTH_VECTOR.push_back(input);
    SMOOTH_VECTOR.push_back(sigma);
    SMOOTH_VECTOR.push_back(output);
    SMOOTH_VECTOR.push_back(std::to_string((int) is_sigma_in_spacing_units));
    SMOOTH_VECTOR.push_back(std::to_string((int) use_median_filtering));

    return ants::SmoothImage(SMOOTH_VECTOR, &std::cout);
}
