/**
 * @file main.cpp
 * @author TIMOTHY ANDERSON (SIRTWINKLEBERRY.COM)
 * @brief 
 * @version 1.2
 * @date 2024-03-04
 * 
 * @copyright GPLv3 (c) 2024
 * 
 * @todo fill docstrings
 * @todo print_usage() function
 * 
 */

#ifdef _WIN32
#include <windows.h>
using access = _access;
#endif

#ifdef __unix__
#include <unistd.h>
#endif

#include <typeindex>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <cstdlib>
#include <boost/timer/timer.hpp>

#include "externals/include/RNifti.h"
#include "externals/include/json.hpp"
#include "externals/include/Eigen/Core"

#include "src/mp2rage.hpp"
#include "src/interpolate.hpp"
#include "src/utils.hpp"
#include "src/normalize.hpp"
#include "src/denoise.hpp"
#include "src/preprocessing.hpp"


/**
 * @brief 
 * 
 */
void print_usage()
{

}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 */
void print_command(int argc, char const *argv[])
{
    std::string argument;
    std::for_each( argv, argv + argc , [&]( const char* c_str ){ argument += std::string ( c_str ) + " "; } );
    std::cout << "COMMAND >> " + argument << std::endl;
}

/**
 * @brief 
 * 
 * @param path 
 * @param status 
 * @return true 
 * @return false 
 */
bool file_exists(const std::string &path, std::filesystem::file_status status = std::filesystem::file_status{})
{
    if (std::filesystem::status_known(status) ? std::filesystem::exists(status) : std::filesystem::exists(std::filesystem::path(path)))
        return true;
    return false;
}

/**
 * @brief 
 * 
 * @param path 
 * @return true 
 * @return false 
 */
bool parent_is_writable(const std::string &path)
{
    if (access(std::filesystem::path(path).parent_path().c_str(), W_OK) == 0)
        return true;
    return false;
}

/**
 * @brief 
 * 
 * @param config 
 * @param verbose 
 */
void check_type_validity_of_parameters(const nlohmann::json &config, bool verbose = true)
{
    std::string expected_type = "";
    std::vector<double> vec;
    
    enum TypeID {
        BOOL
        , INPUT
        , OUTPUT
        , STRING
        , FLOAT
        , INT
        , RANGE
    };

    std::map<std::string, TypeID> map {
        /* BOOL PARAMETERS */
        {"verbose", BOOL}

        , {"use_deprecated_normalization_from_12bits", BOOL}
        , {"use_alternative_denoising_on_synthetic_maps", BOOL}
        , {"do_transform_B1_map_to_t1wUNI_space", BOOL}
        , {"do_smoothing_of_B1_map_in_t1wUNI_space", BOOL}
        , {"do_smoothing_using_median_filtering", BOOL}
        , {"is_ants_smoothing_sigma_in_spacing_units", BOOL}
        , {"do_mask_outside_valid_qT1_interpolation_range", BOOL}
        , {"do_bound_b1_to_valid_interpolation_range", BOOL}
        , {"do_round_on_export", BOOL}

        , {"compute_t1wUNI_DEN", BOOL}
        , {"compute_t1wUNI_B1Corrected", BOOL}
        , {"compute_t1wUNI_B1Corrected_DEN", BOOL}
        , {"compute_qT1", BOOL}
        , {"compute_qR1", BOOL}
        , {"compute_EDGE", BOOL}
        , {"compute_EDGE_DEN", BOOL}
        , {"compute_FLAWS", BOOL}
        , {"compute_FLAWS_DEN", BOOL}

        /* STRING PARAMETERS */
        , {"path_INPUT_b1_faUnit", INPUT}
        , {"path_INPUT_inversion_1_msUnit", INPUT}
        , {"path_INPUT_inversion_2_msUnit", INPUT}
        , {"path_INPUT_t1wUNI_dicomUnit", INPUT}

        , {"path_OUTPUT_b1_resliced_faUnit", OUTPUT}
        , {"path_OUTPUT_b1_resliced_smoothed_faUnit", OUTPUT}
        , {"path_OUTPUT_b1_processed_prct", OUTPUT}
        , {"path_OUTPUT_t1wUNI_DEN_dicomUnit", OUTPUT}
        , {"path_OUTPUT_t1wUNI_B1Corrected_dicomUnit", OUTPUT}
        , {"path_OUTPUT_t1wUNI_B1Corrected_DEN_dicomUnit", OUTPUT}
        , {"path_OUTPUT_qT1_msUnit", OUTPUT}
        , {"path_OUTPUT_qR1_pksUnit", OUTPUT}
        , {"path_OUTPUT_EDGE_dicomUnit", OUTPUT}
        , {"path_OUTPUT_EDGE_DEN_dicomUnit", OUTPUT}
        , {"path_OUTPUT_FLAWS_dicomUnit", OUTPUT}
        , {"path_OUTPUT_FLAWS_DEN_dicomUnit", OUTPUT}

        , {"ants_interpolation_method_for_resampling", STRING}
        , {"ants_smoothing_sigma", STRING}

        /* DOUBLE PARAMETERS */
        , {"vref_b1_vUnit", FLOAT}
        , {"vref_t1wUNI_vUnit", FLOAT}
        , {"target_b1_faUnit", FLOAT}
        , {"noise_shift", FLOAT}
        // MP2RAGE UNI parameters
        , {"t_echo_spacing_msUnit", FLOAT}
        , {"t_repeat_MP2RAGE_msUnit", FLOAT}
        , {"t_inversion1_msUnit", FLOAT}
        , {"t_inversion2_msUnit", FLOAT}
        , {"fa_1_degUnit", FLOAT}
        , {"fa_2_degUnit", FLOAT}
        , {"inversion_efficiency", FLOAT}
        , {"M0", FLOAT}
        // MP2RAGE Synthetic EDGE parameters
        , {"edge_t_echo_spacing_msUnit", FLOAT}
        , {"edge_t_repeat_MP2RAGE_msUnit", FLOAT}
        , {"edge_t_inversion1_msUnit", FLOAT}
        , {"edge_t_inversion2_msUnit", FLOAT}
        , {"edge_fa_1_degUnit", FLOAT}
        , {"edge_fa_2_degUnit", FLOAT}
        , {"edge_inversion_efficiency", FLOAT}
        , {"edge_M0", FLOAT}
        // MP2RAGE Synthetic FLAWS 1 parameters
        , {"flaws1_t_echo_spacing_msUnit", FLOAT}
        , {"flaws1_t_repeat_MP2RAGE_msUnit", FLOAT}
        , {"flaws1_t_inversion1_msUnit", FLOAT}
        , {"flaws1_t_inversion2_msUnit", FLOAT}
        , {"flaws1_fa_1_degUnit", FLOAT}
        , {"flaws1_fa_2_degUnit", FLOAT}
        , {"flaws1_inversion_efficiency", FLOAT}
        , {"flaws1_M0", FLOAT}
        // MP2RAGE Synthetic FLAWS 2 parameters
        , {"flaws2_t_echo_spacing_msUnit", FLOAT}
        , {"flaws2_t_repeat_MP2RAGE_msUnit", FLOAT}
        , {"flaws2_t_inversion1_msUnit", FLOAT}
        , {"flaws2_t_inversion2_msUnit", FLOAT}
        , {"flaws2_fa_1_degUnit", FLOAT}
        , {"flaws2_fa_2_degUnit", FLOAT}
        , {"flaws2_inversion_efficiency", FLOAT}
        , {"flaws2_M0", FLOAT}

        /* INT PARAMETERS */
        , {"datatype", INT}
        , {"n_threads", INT}
        // MP2RAGE UNI parameters
        , {"n_before", INT}
        , {"n_after", INT}
        // MP2RAGE Synthetic EDGE parameters
        , {"edge_n_before", INT}
        , {"edge_n_after", INT}
        // MP2RAGE Synthetic FLAWS 1 parameters
        , {"flaws1_n_before", INT}
        , {"flaws1_n_after", INT}
        // MP2RAGE Synthetic FLAWS 2 parameters
        , {"flaws2_n_before", INT}
        , {"flaws2_n_after", INT}

        /* RANGE PARAMETERS */
        , {"array_b1_relativeUnit", RANGE}
        , {"array_qT1_msUnit", RANGE}
    };

    for (const auto &[key, value] : map)
    {
        try
        {
            if (config[key].is_null()) throw std::invalid_argument(key + " is null. Check its presence in the configuration file.");
            
            switch (value)
            {
                case BOOL:
                    expected_type = "boolean";
                    if ( !config[key].is_boolean() ) throw std::invalid_argument(key + " has wrong type.");
                    break;
                case INPUT:
                    expected_type = "string (input filepath)";
                    if ( !config[key].is_string() ) throw std::invalid_argument(key + " has wrong type.");
                    if ( !file_exists(config[key].template get<std::string>()) ) throw std::invalid_argument(key + " file does not exist.");
                    break;
                case OUTPUT:
                    expected_type = "string (output filepath)";
                    if ( !config[key].is_string() ) throw std::invalid_argument(key + " has wrong type.");
                    if ( !parent_is_writable(config[key].template get<std::string>()) ) throw std::invalid_argument(key + " location is not writable.");
                    break;
                case STRING:
                    expected_type = "string";
                    if ( !config[key].is_string() ) throw std::invalid_argument(key + " has wrong type.");
                    break;
                case FLOAT:
                    expected_type = "positive real";
                    if ( !config[key].is_number() ) throw std::invalid_argument(key + " has wrong type.");
                    if ( config[key].template get<double>() < 0) throw std::invalid_argument(key + " should be positive.");
                    break;
                case INT:
                    expected_type = "positive integer";
                    if ( !config[key].is_number_integer() && !config[key].is_number_unsigned() ) throw std::invalid_argument(key + " has wrong type.");
                    if ( config[key].template get<int>() < 0) throw std::invalid_argument(key + " should be positive.");
                    break;
                case RANGE:
                    expected_type = "3-elements list (# of pts, min, max)";
                    if ( !config[key].is_array() ) throw std::invalid_argument(key + " has wrong type.");
                    vec = config[key].template get<std::vector<double>>();
                    if ( vec.size() != 3 ) throw std::invalid_argument(key + " should have 3 elements.");
                    break;
                default:
                    throw std::invalid_argument(key + " has wrong type.");
                    break;
            }

            std::cout << key << " = " << config[key].dump() << "\n";
            // std::cout << config[key] << " - " << typeid(config[key]).name() << "\n";
        }
        catch (const std::exception &e)
        {
            std::cout << "\033[1;31m" << e.what() << "\033[0m\n";
            std::cout << "\033[1;32m" << "RECEIVED >> " << "\033[0m" << config[key].dump() << std::endl;
            throw std::invalid_argument(key + " expects " + expected_type + ".");
        }
    }

    std::cout << std::endl;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char const *argv[])
{
    bool VERBOSE;

    {
        boost::timer::auto_cpu_timer timer;
        nlohmann::json config;

        try
        {
            if (argc < 2) throw std::runtime_error("Invalid number of arguments passed to the program. Please supply a path to a JSON runtime configuration file.");
            if (argc > 2) throw std::runtime_error("Invalid number of arguments passed to the program. Please only supply a path to a JSON runtime configuration file.");
            std::ifstream file(argv[1]);
            config = nlohmann::json::parse(file);
            check_type_validity_of_parameters(config, true);
        }
        catch (const std::exception &e)
        {
            print_usage();
            std::cout << e.what() << std::endl;
            print_command(argc, argv);
            return EXIT_FAILURE;
        }


        /*
            SOME CONSTANTS
         */
        VERBOSE = config["verbose"].template get<bool>();
        bool USE_DEPRECATED = config["use_deprecated_normalization_from_12bits"].template get<bool>();
        bool UNIFIED_DENOISE = !config["use_alternative_denoising_on_synthetic_maps"].template get<bool>();
        bool DO_ROUND = config["do_round_on_export"].template get<bool>();
        int DATATYPE = config["datatype"].template get<int>();
        int N_THREADS = config["n_threads"].template get<int>();
        std::string PATH_B1_MAP = config["path_INPUT_b1_faUnit"].template get<std::string>();
        const std::vector<double> RANGE_B1 = config["array_b1_relativeUnit"].template get<std::vector<double>>();
        const std::vector<double> RANGE_T1 = config["array_qT1_msUnit"].template get<std::vector<double>>();

        /*
            BRINGING VARIABLES INTO SCOPE
            KEEPING THEM UNTIL END OF EXPORT
         */
        Eigen::ArrayXd eigen_B1_in_UNI_SPACE_relative;
        Eigen::ArrayXd eigen_T1W_INV1_0_to_4095;
        Eigen::ArrayXd eigen_T1W_INV2_0_to_4095;
        Eigen::ArrayXd eigen_T1W_UNI_centered;
        Eigen::ArrayXd data_QT1_in_unit;
        Eigen::ArrayXd data_QR1_in_pkunit;
        Eigen::ArrayXd data_T1W_UNI_0_TO_4095_denoised;
        Eigen::ArrayXd data_T1W_UNI_CORRECTED_0_TO_4095_masked;
        Eigen::ArrayXd data_T1W_UNI_CORRECTED_0_TO_4095_denoised_masked;
        Eigen::ArrayXd SYN_EDGE_0_TO_4095;
        Eigen::ArrayXd SYN_EDGE_0_TO_4095_denoised;
        Eigen::ArrayXd SYN_FLAWS_0_TO_4095_masked;
        Eigen::ArrayXd SYN_FLAWS_0_TO_4095_masked_denoised;

        std::vector<std::pair<std::string, Eigen::ArrayXd*>> export_vector;
        std::pair<double, double> bijectivity_range;

        
        /* 
            PREPROCESSING THE MAP: B1
            TRANSFORMING IT TO T1W UNI SPACE
            APPLYING A GAUSSIAN SMOOTH TO IT
         */
        if ( config["do_transform_B1_map_to_t1wUNI_space"].template get<bool>() )
        {
            if ( ANTS_APPLY_TRANSFORMS(
                    PATH_B1_MAP
                    , config["path_INPUT_t1wUNI_dicomUnit"].template get<std::string>()
                    , config["path_OUTPUT_b1_resliced_faUnit"].template get<std::string>()
                    , "default"
                    , "identity"
                    , config["ants_interpolation_method_for_resampling"].template get<std::string>()
                    , 0.0
                    , 3
                    , 0
                    , VERBOSE)
                == EXIT_SUCCESS )
                PATH_B1_MAP = config["path_OUTPUT_b1_resliced_faUnit"].template get<std::string>();
            else std::cout << "\033[1;31mFailed to apply transforms. Attempting to continue without.\033[0m" << std::endl;

        }

        if ( config["do_smoothing_of_B1_map_in_t1wUNI_space"].template get<bool>() )
        {
            if ( ANTS_SMOOTH_IMAGE(
                    PATH_B1_MAP
                    , config["path_OUTPUT_b1_resliced_smoothed_faUnit"].template get<std::string>()
                    , 3
                    , config["ants_smoothing_sigma"].template get<std::string>()
                    , config["is_ants_smoothing_sigma_in_spacing_units"].template get<bool>()
                    , config["do_smoothing_using_median_filtering"].template get<bool>()
                    , VERBOSE)
                == EXIT_SUCCESS )
                PATH_B1_MAP = config["path_OUTPUT_b1_resliced_smoothed_faUnit"].template get<std::string>();
            else std::cout << "\033[1;31mFailed to apply Gaussian smoothing. Continuing without.\033[0m" << std::endl;
        }


        /* 
            LOADING THE MAP: T1W UNI
            MAKING IT INTO AN EIGEN ARRAY OF DOUBLE
            RESCALING IT TO UNIT CUBE CENTERED AT 0 ([-.5, .5])
         */
        const RNifti::NiftiImage volume_T1W_UNI_0_to_4095 = RNifti::NiftiImage(
            config["path_INPUT_t1wUNI_dicomUnit"].template get<std::string>()
            , true
        );
        std::vector<double> VECTOR = volume_T1W_UNI_0_to_4095.getData<double>();
        const size_t SIZE = VECTOR.size();

        #pragma omp parallel num_threads(std::min(N_THREADS, 4)) shared(SIZE, VECTOR, config, eigen_T1W_UNI_centered, eigen_B1_in_UNI_SPACE_relative, eigen_T1W_INV1_0_to_4095, eigen_T1W_INV2_0_to_4095)
        #pragma omp single
        {
            #pragma omp task
            {
                if ( USE_DEPRECATED )
                    eigen_T1W_UNI_centered = TO_UNI_RANGE_DEPRECATED<Eigen::ArrayXd, Eigen::ArrayXd>(
                        Eigen::Map<Eigen::ArrayXd, Eigen::Unaligned>(
                            VECTOR.data(),
                            SIZE
                        )
                        , VERBOSE
                    );
                else
                    eigen_T1W_UNI_centered = TO_UNI_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(
                        Eigen::Map<Eigen::ArrayXd, Eigen::Unaligned>(
                            VECTOR.data(),
                            SIZE
                        )
                        , VERBOSE
                    );
            }


            /* 
                LOADING THE MAP: B1
                MAKING IT INTO AN EIGEN ARRAY OF DOUBLE
                RESCALING IT TO RELATIVE UNITS
                /!\ WARNING: Currently, assuming that B1 size == T1w size (pre-registration)
             */
            #pragma omp task
            {
                const RNifti::NiftiImage volume_B1_in_UNI_SPACE_0_to_4095 = RNifti::NiftiImage(
                    PATH_B1_MAP
                    , true
                );
                eigen_B1_in_UNI_SPACE_relative = B1_TO_RELATIVE_B1<Eigen::ArrayXd, Eigen::ArrayXd>(
                    Eigen::Map<Eigen::ArrayXd, Eigen::Unaligned>(
                        volume_B1_in_UNI_SPACE_0_to_4095.getData<double>().data(),
                        SIZE
                    )
                    , config["target_b1_faUnit"].template get<double>()
                    , config["vref_b1_vUnit"].template get<double>()
                    , config["vref_t1wUNI_vUnit"].template get<double>()
                    , VERBOSE
                );

                if ( config["do_bound_b1_to_valid_interpolation_range"].template get<bool>() )
                    eigen_B1_in_UNI_SPACE_relative = MASK_FROM_RANGE<Eigen::ArrayXd>(
                        eigen_B1_in_UNI_SPACE_relative
                        , RANGE_B1.at(1)
                        , RANGE_B1.at(1)
                        , RANGE_B1.at(2)
                        , RANGE_B1.at(2)
                        , VERBOSE
                    );
            }


            /* 
                LOAD INVERSION MAPS ONLY IF DENOISE ALGORITHM NEEDED
                TO LOWER MEMORY USE AND I/O RUNTIME LOSS
                NEED TO BRING eigen_T1W_INV[1/2]_0_to_4095 TO SCOPE THOUGH
             */
            if ( config["compute_t1wUNI_DEN"].template get<bool>()
                || config["compute_t1wUNI_B1Corrected_DEN"].template get<bool>()
                || config["compute_EDGE_DEN"].template get<bool>()
                || config["compute_FLAWS_DEN"].template get<bool>() )
            {
                /* 
                    LOADING THE MAP: T1W INV 1
                    MAKING IT INTO AN EIGEN ARRAY OF DOUBLE
                 */
                #pragma omp task
                {
                    const RNifti::NiftiImage volume_T1W_INV1_0_to_4095 = RNifti::NiftiImage(
                        config["path_INPUT_inversion_1_msUnit"].template get<std::string>()
                        , true
                    );
                    eigen_T1W_INV1_0_to_4095 = Eigen::Map<Eigen::ArrayXd, Eigen::Unaligned>(
                        volume_T1W_INV1_0_to_4095.getData<double>().data()
                        , SIZE
                    );
                }


                /* 
                    LOADING THE MAP: T1W INV 2
                    MAKING IT INTO AN EIGEN ARRAY OF DOUBLE
                 */
                #pragma omp task
                {
                    const RNifti::NiftiImage volume_T1W_INV2_0_to_4095 = RNifti::NiftiImage(
                        config["path_INPUT_inversion_2_msUnit"].template get<std::string>()
                        , true
                    );
                    eigen_T1W_INV2_0_to_4095 = Eigen::Map<Eigen::ArrayXd, Eigen::Unaligned>(
                        volume_T1W_INV2_0_to_4095.getData<double>().data()
                        , SIZE
                    );
                }
            }

            #pragma omp taskwait
        }

        
        /* 
            PREPARING: INTERPOLANT
            - DEFINING 2 RANGES: (X (no unit), Y (ms))
            - INITIALIZING INTERPOLANT FROM THESE RANGES
         */
        const Eigen::ArrayXd B1VectorRange_relative = Eigen::ArrayXd::LinSpaced(
            (int) RANGE_B1.at(0)
            , RANGE_B1.at(1)
            , RANGE_B1.at(2))
        ;
        const Eigen::ArrayXd QT1VectorRange_in_unit = Eigen::ArrayXd::LinSpaced(
            (int) RANGE_T1.at(0)
            , RANGE_T1.at(1)
            , RANGE_T1.at(2)
        );

        auto interp = INIT_INTERPOLATOR_IN_UNIT<double, Eigen::ArrayXd, Eigen::ArrayXd>(
            B1VectorRange_relative
            , QT1VectorRange_in_unit
            , config["t_inversion1_msUnit"].template get<double>()
            , config["t_inversion2_msUnit"].template get<double>()
            , config["t_repeat_MP2RAGE_msUnit"].template get<double>()
            , config["t_echo_spacing_msUnit"].template get<double>()
            , config["n_before"].template get<int>()
            , config["n_after"].template get<int>()
            , config["fa_1_degUnit"].template get<double>()
            , config["fa_2_degUnit"].template get<double>()
            , config["inversion_efficiency"].template get<double>()
            , config["M0"].template get<double>()
            , &bijectivity_range
            , VERBOSE
        );


        /* 
            GENERATING THE MAP: QT1 (ms)
            MAKING IT INTO AN STD VECTOR OF DOUBLE
            MARKING IT FOR EXPORT IN `DATATYPE` WITH INTEGER ROUNDING IF `DO_ROUND`
         */
        data_QT1_in_unit = COMPUTE_QT1MAP_IN_UNIT<double, Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(
            interp
            , eigen_B1_in_UNI_SPACE_relative
            , eigen_T1W_UNI_centered
            , N_THREADS
            , VERBOSE
        );

        if ( config["do_mask_outside_valid_qT1_interpolation_range"].template get<bool>() )
            data_QT1_in_unit = MASK_FROM_RANGE<Eigen::ArrayXd>(
                data_QT1_in_unit
                , bijectivity_range.first
                , 0
                , bijectivity_range.second
                , 0
                , VERBOSE
            );

        if ( config["compute_qT1"].template get<bool>() )
        {
            export_vector.push_back(
                std::pair<std::string, Eigen::ArrayXd*>(
                    config["path_OUTPUT_qT1_msUnit"].template get<std::string>()
                    , &data_QT1_in_unit
                )
            );
        }


        /* 
            GENERATING THE MAP: QR1 ((ms)^-1) AND RESCALING IT BY 10^6 ((ms)^-1 -> (ks)^-1)
            MAKING IT INTO AN STD VECTOR OF DOUBLE
            MARKING IT FOR EXPORT IN `DATATYPE` WITH INTEGER ROUNDING IF `DO_ROUND`
         */
        if ( config["compute_qR1"].template get<bool>() )
        {
            Eigen::ArrayXd data_QR1_in_pkunit = (Eigen::ArrayXd) (1e6 * COMPUTE_QR1MAP_IN_PER_UNIT<Eigen::ArrayXd, Eigen::ArrayXd>(
                data_QT1_in_unit.array()
                , VERBOSE
            ).array());

            export_vector.push_back(
                std::pair<std::string, Eigen::ArrayXd*>(
                    config["path_OUTPUT_qR1_pksUnit"].template get<std::string>()
                    , &data_QR1_in_pkunit
                )
            );
        }


        /* 
            GENERATING THE MAP: T1W UNI - BACK B1-CORRECTED
            RESCALING IT
            MASKING IT
            MAKING IT INTO AN STD VECTOR OF DOUBLE
            MARKING IT FOR EXPORT IN `DATATYPE` WITH INTEGER ROUNDING IF `DO_ROUND`
         */
        if ( config["compute_t1wUNI_B1Corrected"].template get<bool>() || config["compute_t1wUNI_B1Corrected_DEN"].template get<bool>() )
        {
            Eigen::ArrayXd data_T1W_UNI_CORRECTED_centered = COMPUTE_BACK_B1CORRECTED_T1W_UNIMAP_CENTERED<Eigen::ArrayXd, Eigen::ArrayXd>(
                data_QT1_in_unit
                , config["t_inversion1_msUnit"].template get<double>()
                , config["t_inversion2_msUnit"].template get<double>()
                , config["t_repeat_MP2RAGE_msUnit"].template get<double>()
                , config["t_echo_spacing_msUnit"].template get<double>()
                , config["n_before"].template get<int>()
                , config["n_after"].template get<int>()
                , config["fa_1_degUnit"].template get<double>()
                , config["fa_2_degUnit"].template get<double>()
                , config["inversion_efficiency"].template get<double>()
                , config["M0"].template get<double>()
                , N_THREADS
                , VERBOSE
            );

            if ( config["compute_t1wUNI_B1Corrected"].template get<bool>() )
            {
                Eigen::ArrayXd data_T1W_UNI_CORRECTED_0_TO_4095;

                if ( USE_DEPRECATED )
                    data_T1W_UNI_CORRECTED_0_TO_4095 = TO_12BITS_RANGE_DEPRECATED<Eigen::ArrayXd, Eigen::ArrayXd>(data_T1W_UNI_CORRECTED_centered.matrix().reshaped(), VERBOSE);
                else
                    data_T1W_UNI_CORRECTED_0_TO_4095 = TO_12BITS_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(data_T1W_UNI_CORRECTED_centered.matrix().reshaped(), VERBOSE);

                data_T1W_UNI_CORRECTED_0_TO_4095_masked = MASK_FROM_REFERENCE<Eigen::ArrayXd, Eigen::ArrayXd>(data_T1W_UNI_CORRECTED_0_TO_4095, data_QT1_in_unit, VERBOSE);

                export_vector.push_back(
                    std::pair<std::string, Eigen::ArrayXd*>(
                        config["path_OUTPUT_t1wUNI_B1Corrected_dicomUnit"].template get<std::string>()
                        , &data_T1W_UNI_CORRECTED_0_TO_4095_masked
                    )
                );
            }
        
            /* 
                GENERATING THE MAP: T1W UNI - BACK B1-CORRECTED AND DENOISED
                RESCALING IT
                MAKING IT INTO AN STD VECTOR OF DOUBLE
                MARKING IT FOR EXPORT IN `DATATYPE` WITH INTEGER ROUNDING IF `DO_ROUND`
             */
            if ( config["compute_t1wUNI_B1Corrected_DEN"].template get<bool>() )
            {
                Eigen::ArrayXd data_T1W_UNI_CORRECTED_0_TO_4095_denoised;

                Eigen::ArrayXd data_T1W_UNI_CORRECTED_centered_denoised = DENOISE<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(
                    data_T1W_UNI_CORRECTED_centered.matrix().reshaped()
                    , eigen_T1W_INV1_0_to_4095
                    , eigen_T1W_INV2_0_to_4095
                    , config["noise_shift"].template get<double>()
                    , VERBOSE);

                if ( USE_DEPRECATED )
                    data_T1W_UNI_CORRECTED_0_TO_4095_denoised = TO_12BITS_RANGE_DEPRECATED<Eigen::ArrayXd, Eigen::ArrayXd>(data_T1W_UNI_CORRECTED_centered_denoised.matrix().reshaped(), VERBOSE);
                else
                    data_T1W_UNI_CORRECTED_0_TO_4095_denoised = TO_12BITS_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(data_T1W_UNI_CORRECTED_centered_denoised.matrix().reshaped(), VERBOSE);

                data_T1W_UNI_CORRECTED_0_TO_4095_denoised_masked = MASK_FROM_REFERENCE<Eigen::ArrayXd, Eigen::ArrayXd>(data_T1W_UNI_CORRECTED_0_TO_4095_denoised, data_QT1_in_unit, VERBOSE);
        
                export_vector.push_back(
                    std::pair<std::string, Eigen::ArrayXd*>(
                        config["path_OUTPUT_t1wUNI_B1Corrected_DEN_dicomUnit"].template get<std::string>()
                        , &data_T1W_UNI_CORRECTED_0_TO_4095_denoised_masked
                    )
                );
            }
        }


        /* 
            GENERATING THE MAP: T1W UNI (ORIGINAL) - DENOISED
            RESCALING IT
            MAKING IT INTO AN STD VECTOR OF DOUBLE
            MARKING IT FOR EXPORT IN `DATATYPE` WITH INTEGER ROUNDING IF `DO_ROUND`
         */
        if ( config["compute_t1wUNI_DEN"].template get<bool>() )
        {
            Eigen::ArrayXd eigen_T1W_UNI_centered_denoised = DENOISE<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(
                eigen_T1W_UNI_centered.matrix().reshaped()
                , eigen_T1W_INV1_0_to_4095
                , eigen_T1W_INV2_0_to_4095
                , config["noise_shift"].template get<double>()
                , VERBOSE);
            
            if ( USE_DEPRECATED )
                data_T1W_UNI_0_TO_4095_denoised = TO_12BITS_RANGE_DEPRECATED<Eigen::ArrayXd, Eigen::ArrayXd>(eigen_T1W_UNI_centered_denoised.matrix().reshaped(), VERBOSE);
            else
                data_T1W_UNI_0_TO_4095_denoised = TO_12BITS_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(eigen_T1W_UNI_centered_denoised.matrix().reshaped(), VERBOSE);

            export_vector.push_back(
                std::pair<std::string, Eigen::ArrayXd*>(
                    config["path_OUTPUT_t1wUNI_DEN_dicomUnit"].template get<std::string>()
                    , &data_T1W_UNI_0_TO_4095_denoised
                )
            );
        }


        /* 
            GENERATING THE MAP: SYNTHETIC EDGE
            USING DEFAULT PARAMETRIZATION
            RESCALING IT
            MAKING IT INTO AN STD VECTOR OF DOUBLE
            MARKING IT FOR EXPORT IN `DATATYPE` WITH INTEGER ROUNDING IF `DO_ROUND`
         */
        if ( config["compute_EDGE"].template get<bool>() || config["compute_EDGE_DEN"].template get<bool>() )
        {
            Eigen::ArrayXd SYN_EDGE = EDGE_CENTERED<Eigen::ArrayXd, Eigen::ArrayXd>(
                data_QT1_in_unit.matrix().reshaped()
                , config["edge_t_inversion1_msUnit"].template get<double>()
                , config["edge_t_inversion2_msUnit"].template get<double>()
                , config["edge_t_repeat_MP2RAGE_msUnit"].template get<double>()
                , config["edge_t_echo_spacing_msUnit"].template get<double>()
                , config["edge_n_before"].template get<int>()
                , config["edge_n_after"].template get<int>()
                , config["edge_fa_1_degUnit"].template get<double>()
                , config["edge_fa_2_degUnit"].template get<double>()
                , config["edge_inversion_efficiency"].template get<double>()
                , config["edge_M0"].template get<double>()
                , N_THREADS
                , VERBOSE);

            if ( USE_DEPRECATED )
                SYN_EDGE_0_TO_4095 = TO_12BITS_RANGE_DEPRECATED<Eigen::ArrayXd, Eigen::ArrayXd>(SYN_EDGE.matrix().reshaped(), VERBOSE);
            else
                SYN_EDGE_0_TO_4095 = TO_12BITS_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(SYN_EDGE.matrix().reshaped(), VERBOSE);

            if ( config["compute_EDGE"].template get<bool>())
            {
                export_vector.push_back(
                    std::pair<std::string, Eigen::ArrayXd*>(
                        config["path_OUTPUT_EDGE_dicomUnit"].template get<std::string>()
                        , &SYN_EDGE_0_TO_4095
                    )
                );
            }

            /* 
                GENERATING THE MAP: SYNTHETIC EDGE - DENOISED
                MAKING IT INTO AN STD VECTOR OF DOUBLE
                MARKING IT FOR EXPORT IN `DATATYPE` WITH INTEGER ROUNDING IF `DO_ROUND`
             */
            if ( config["compute_EDGE_DEN"].template get<bool>() )
            {
                if ( UNIFIED_DENOISE )
                {
                    Eigen::ArrayXd SYN_EDGE_denoised = DENOISE<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(
                        SYN_EDGE.matrix().reshaped()
                        , eigen_T1W_INV1_0_to_4095
                        , eigen_T1W_INV2_0_to_4095
                        , config["noise_shift"].template get<double>()
                        , VERBOSE);

                    if ( USE_DEPRECATED )
                        SYN_EDGE_0_TO_4095_denoised = TO_12BITS_RANGE_DEPRECATED<Eigen::ArrayXd, Eigen::ArrayXd>(SYN_EDGE_denoised.matrix().reshaped(), VERBOSE);
                    else
                        SYN_EDGE_0_TO_4095_denoised = TO_12BITS_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(SYN_EDGE_denoised.matrix().reshaped(), VERBOSE);
                }
                else
                    SYN_EDGE_0_TO_4095_denoised = DENOISE_ALT<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(
                        SYN_EDGE_0_TO_4095.matrix().reshaped()
                        , eigen_T1W_INV1_0_to_4095
                        , eigen_T1W_INV2_0_to_4095
                        , config["noise_shift"].template get<double>()
                        , VERBOSE);

                export_vector.push_back(
                    std::pair<std::string, Eigen::ArrayXd*>(
                        config["path_OUTPUT_EDGE_DEN_dicomUnit"].template get<std::string>()
                        , &SYN_EDGE_0_TO_4095_denoised
                    )
                );
            }
        }


        /* 
            GENERATING THE MAP: SYNTHETIC FLAWS
            USING DEFAULT PARAMETRIZATION
            RESCALING IT
            MASKING IT
            MAKING IT INTO AN STD VECTOR OF DOUBLE
            MARKING IT FOR EXPORT IN `DATATYPE` WITH INTEGER ROUNDING IF `DO_ROUND`
         */
        if ( config["compute_FLAWS"].template get<bool>() || config["compute_EDGE_DEN"].template get<bool>() )
        {
            Eigen::ArrayXd SYN_FLAWS_0_TO_4095;

            Eigen::ArrayXd SYN_FLAWS = FLAWS_CENTERED<Eigen::ArrayXd, Eigen::ArrayXd>(
                data_QT1_in_unit.matrix().reshaped()
                , config["flaws1_t_inversion1_msUnit"].template get<double>()
                , config["flaws1_t_inversion2_msUnit"].template get<double>()
                , config["flaws1_t_repeat_MP2RAGE_msUnit"].template get<double>()
                , config["flaws1_t_echo_spacing_msUnit"].template get<double>()
                , config["flaws1_n_before"].template get<int>()
                , config["flaws1_n_after"].template get<int>()
                , config["flaws1_fa_1_degUnit"].template get<double>()
                , config["flaws1_fa_2_degUnit"].template get<double>()
                , config["flaws1_inversion_efficiency"].template get<double>()
                , config["flaws1_M0"].template get<double>()
                , config["flaws2_t_inversion1_msUnit"].template get<double>()
                , config["flaws2_t_inversion2_msUnit"].template get<double>()
                , config["flaws2_t_repeat_MP2RAGE_msUnit"].template get<double>()
                , config["flaws2_t_echo_spacing_msUnit"].template get<double>()
                , config["flaws2_n_before"].template get<int>()
                , config["flaws2_n_after"].template get<int>()
                , config["flaws2_fa_1_degUnit"].template get<double>()
                , config["flaws2_fa_2_degUnit"].template get<double>()
                , config["flaws2_inversion_efficiency"].template get<double>()
                , config["flaws2_M0"].template get<double>()
                , N_THREADS
                , VERBOSE);

            if ( USE_DEPRECATED )
                SYN_FLAWS_0_TO_4095 = TO_12BITS_RANGE_DEPRECATED<Eigen::ArrayXd, Eigen::ArrayXd>(SYN_FLAWS.matrix().reshaped(), VERBOSE);
            else
                SYN_FLAWS_0_TO_4095 = TO_12BITS_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(SYN_FLAWS.matrix().reshaped(), VERBOSE);

            SYN_FLAWS_0_TO_4095_masked = MASK_FROM_REFERENCE<Eigen::ArrayXd, Eigen::ArrayXd>(SYN_FLAWS_0_TO_4095, data_QT1_in_unit, VERBOSE);

            if ( config["compute_FLAWS"].template get<bool>() )
            {
                export_vector.push_back(
                    std::pair<std::string, Eigen::ArrayXd*>(
                        config["path_OUTPUT_FLAWS_dicomUnit"].template get<std::string>()
                        , &SYN_FLAWS_0_TO_4095_masked
                    )
                );
            }

            /* 
                GENERATING THE MAP: SYNTHETIC FLAWS - DENOISED
                MAKING IT INTO AN STD VECTOR OF DOUBLE
                MARKING IT FOR EXPORT IN `DATATYPE` WITH INTEGER ROUNDING IF `DO_ROUND`
             */
            if ( config["compute_FLAWS_DEN"].template get<bool>() )
            {
                if ( UNIFIED_DENOISE )
                {
                    Eigen::ArrayXd SYN_FLAWS_centered_masked;

                    if ( USE_DEPRECATED )
                        SYN_FLAWS_centered_masked = TO_UNI_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(SYN_FLAWS_0_TO_4095_masked.matrix().reshaped(), VERBOSE);
                    else
                        SYN_FLAWS_centered_masked = TO_UNI_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(SYN_FLAWS_0_TO_4095_masked.matrix().reshaped(), VERBOSE);

                    Eigen::ArrayXd SYN_FLAWS_centered_masked_denoised = DENOISE<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(
                        SYN_FLAWS_centered_masked.matrix().reshaped()
                        , eigen_T1W_INV1_0_to_4095
                        , eigen_T1W_INV2_0_to_4095
                        , config["noise_shift"].template get<double>()
                        , VERBOSE);

                    if ( USE_DEPRECATED )
                        SYN_FLAWS_0_TO_4095_masked_denoised = TO_12BITS_RANGE_DEPRECATED<Eigen::ArrayXd, Eigen::ArrayXd>(SYN_FLAWS_centered_masked_denoised.matrix().reshaped(), VERBOSE);
                    else 
                        SYN_FLAWS_0_TO_4095_masked_denoised = TO_12BITS_RANGE<Eigen::ArrayXd, Eigen::ArrayXd>(SYN_FLAWS_centered_masked_denoised.matrix().reshaped(), VERBOSE);
                }
                else
                    SYN_FLAWS_0_TO_4095_masked_denoised = DENOISE_ALT<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(
                        SYN_FLAWS_0_TO_4095_masked.matrix().reshaped()
                        , eigen_T1W_INV1_0_to_4095
                        , eigen_T1W_INV2_0_to_4095
                        , config["noise_shift"].template get<double>()
                        , VERBOSE);

                export_vector.push_back(
                    std::pair<std::string, Eigen::ArrayXd*>(
                        config["path_OUTPUT_FLAWS_DEN_dicomUnit"].template get<std::string>()
                        , &SYN_FLAWS_0_TO_4095_masked_denoised
                    )
                );
            }
        }


        /*
            BRING PROCESSED B1 MAP FROM RELATIVE TO PERCENT LEVELS
            EXPORT ALL MAPS TO DISK
         */
        eigen_B1_in_UNI_SPACE_relative = eigen_B1_in_UNI_SPACE_relative.array() * 100.;
        export_vector.push_back(
            std::pair<std::string, Eigen::ArrayXd*>(
                config["path_OUTPUT_b1_processed_prct"].template get<std::string>()
                , &eigen_B1_in_UNI_SPACE_relative
            )
        );

        EXPORT_RESULTS<Eigen::ArrayXd>(
            export_vector
            , volume_T1W_UNI_0_to_4095
            , DATATYPE
            , DO_ROUND
            , std::min(N_THREADS, (int) export_vector.size())
            , VERBOSE
        );
        
        export_vector.clear();
    }


    /*
        END OF PROGRAM
     */
    if ( VERBOSE )
    {
        std::cout << "\n";
        print_command(argc, argv);
        std::cout << "COMPLETE" << std::endl;
    }

    return EXIT_SUCCESS;
}
