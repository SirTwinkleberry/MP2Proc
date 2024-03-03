#include "src/preprocessing.hpp"

int main(int argc, char const *argv[])
{
    std::string PATH_B1_MAP = "/mnt/c/Users/tanderson/Desktop/b1Correction/cpp/preresliced_01_0008_tfl-b1map-sag-3mm-tar90_c32.nii.gz";
    bool VERBOSE = true;

    if ( true )
    {
        if ( ANTS_APPLY_TRANSFORMS(
                PATH_B1_MAP
                , "/mnt/c/Users/tanderson/Desktop/b1Correction/in/01_0025_t1-mp2r-sag-0p6-p3-cemerem-uni-images_c32.nii.gz"
                , "/mnt/c/Users/tanderson/Desktop/b1Correction/cpp/CPP_b1_resliced_faUnit.nii.gz"
                , "default"
                , "identity"
                , "BSpline[3]"
                , 0.0
                , 3
                , 0
                , VERBOSE)
            == EXIT_SUCCESS )
            PATH_B1_MAP = "/mnt/c/Users/tanderson/Desktop/b1Correction/cpp/CPP_b1_resliced_faUnit.nii.gz";
        else std::cout << "\033[1;31mFailed to apply transforms. Attempting to continue without.\033[0m" << std::endl;

    }

    if ( true )
    {
        if ( ANTS_SMOOTH_IMAGE(
                PATH_B1_MAP
                , "/mnt/c/Users/tanderson/Desktop/b1Correction/cpp/CPP_b1_resliced_smoothed_faUnit.nii.gz"
                , "1x1x1"
                , false
                , false
                , VERBOSE)
            == EXIT_SUCCESS )
            PATH_B1_MAP = "/mnt/c/Users/tanderson/Desktop/b1Correction/cpp/CPP_b1_resliced_smoothed_faUnit.nii.gz";
        else std::cout << "\033[1;31mFailed to apply Gaussian smoothing. Continuing without.\033[0m" << std::endl;
    }

    std::cout << "Output smoothed image: " << PATH_B1_MAP << std::endl;

    return EXIT_SUCCESS;
}
