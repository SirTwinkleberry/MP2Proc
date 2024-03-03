#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

#include "/home/tanderson/tanderson/ants/ANTs/Examples/include/antsApplyTransforms.h"
#include "/home/tanderson/tanderson/ants/ANTs/Examples/include/SmoothImage.h"


int main(int argc, char const *argv[])
{
    std::string input = "/mnt/c/Users/tanderson/Desktop/b1Correction/in/01_0008_tfl-b1map-sag-3mm-tar90_c32.nii.gz";
    std::string reference = "/mnt/c/Users/tanderson/Desktop/b1Correction/in/01_0025_t1-mp2r-sag-0p6-p3-cemerem-uni-images_c32.nii.gz";
    std::string output_reslice = "/mnt/c/Users/tanderson/Desktop/b1Correction/cpp/ants_resliced_01_0008_tfl-b1map-sag-3mm-tar90_c32.nii.gz";
    std::string output_smooth = "/mnt/c/Users/tanderson/Desktop/b1Correction/cpp/ants_smoothed_01_0008_tfl-b1map-sag-3mm-tar90_c32.nii.gz";

    std::string SMOOTHING_SIGMA = "2x2x2";

    std::vector<std::string> reslice;
    std::vector<std::string> smooth;

    reslice.push_back("-v");
    reslice.push_back("1");
    reslice.push_back("-d");
    reslice.push_back("3");
    reslice.push_back("-e");
    reslice.push_back("0");
    reslice.push_back("-u");
    reslice.push_back("int");
    reslice.push_back("default");
    reslice.push_back("-t");
    reslice.push_back("identity");
    reslice.push_back("-n");
    reslice.push_back("BSpline[3]");
    reslice.push_back("-f");
    reslice.push_back("0");
    reslice.push_back("-i");
    reslice.push_back(input);
    reslice.push_back("-r");
    reslice.push_back(reference);
    reslice.push_back("-o");
    reslice.push_back(output_reslice);

    smooth.push_back("3");
    smooth.push_back(output_reslice);
    smooth.push_back(SMOOTHING_SIGMA);
    smooth.push_back(output_smooth);
    smooth.push_back("0");
    smooth.push_back("0");

    ants::antsApplyTransforms(reslice, &std::cout);

    ants::SmoothImage(smooth, &std::cout);

    std::filesystem::remove(output_reslice);

    return 0;
}
