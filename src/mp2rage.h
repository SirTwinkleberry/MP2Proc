#pragma once

#include <cmath>
#include <boost/timer/timer.hpp>
#include <omp.h>

#include "externals/include/Eigen/Dense"


/**
 * @brief Contrarily to the Python and MATLAB implementation, this outputs a matrix of size len(`B1REL`) x len(`T1`)
 * 
 * @tparam T1 
 * @tparam T2 
 * @tparam T3 
 * @param B1REL 
 * @param QT1 
 * @param tInversion1 
 * @param tInversion2 
 * @param TRmp2rage 
 * @param tEchoSpacing 
 * @param nBefore 
 * @param nAfter 
 * @param FA1_deg 
 * @param FA2_deg 
 * @param inversionEfficiency 
 * @param M0 
 * @param verbose 
 * @return T3 
 */
template <typename T1, typename T2, typename T3>
static T3 MP2RAGE(const T1 &B1REL, const T2 &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, bool verbose = true)
{
    boost::timer::auto_cpu_timer timer;

    if (verbose)
    {
        double b1step = B1REL.size() > 1 ? B1REL(1) - B1REL(0) : 0;
        double qt1step = QT1.size() > 1 ? QT1(1) - QT1(0) : 0;

        std::cout << "Computing theoretical T1-weighted MP2RAGE UNI signal" << "\n"
                  << "Matrix version (currently not suitable for dimensions)" << "\n"
                  << "B1REL range: [" << B1REL.array().minCoeff() << ", " << B1REL.array().maxCoeff() << "], step: " << b1step << " (∅)" << "\n"
                  << "  QT1 range: [" << QT1.array().minCoeff() << ", " << QT1.array().maxCoeff() << "], step: " << qt1step << " (in provided time unit)" << "\n"
                  << "Delay to inversion #1: " << tInversion1 << " (in provided time unit)" << "\n"
                  << "Delay to inversion #2: " << tInversion2 << " (in provided time unit)" << "\n"
                  << "MP2RAGE TR           : " << TRmp2rage << " (in provided time unit)" << "\n"
                  << "Echo Spacing         : " << tEchoSpacing << " (in provided time unit)" << "\n"
                  << "# of FLASH 'before'  : " << nBefore << "\n"
                  << "# of FLASH 'after'   : " << nAfter << "\n"
                  << "FLASH Flip Angle #1  : " << FA1_deg << " (°)" << "\n"
                  << "FLASH Flip Angle #2  : " << FA2_deg << " (°)" << "\n"
                  << "Inversion Efficiency : " << inversionEfficiency << "\n"
                  << "M0                   : " << M0
                  << std::endl;
    }

    assert(tInversion1 < tInversion2);
    assert(tInversion2 < TRmp2rage);
    assert((nBefore + nAfter) * tEchoSpacing < TRmp2rage);

    int receptorGain = 1;
    double FA1_rad = FA1_deg * M_PI / 180;
    double FA2_rad = FA2_deg * M_PI / 180;

    int nTotal = nBefore + nAfter;
    double t_a = tInversion1 - nBefore * tEchoSpacing;
    double t_b = tInversion2 - tInversion1 - nTotal * tEchoSpacing;
    double t_c = TRmp2rage - tInversion2 - nAfter * tEchoSpacing;

    Eigen::ArrayXd onesB1 = Eigen::ArrayXd::Ones(B1REL.size());
    Eigen::ArrayXd onesT1 = Eigen::ArrayXd::Ones(QT1.size());
    
    Eigen::ArrayXd QR1 = inverse(QT1.array());

    Eigen::ArrayXd expEcho = exp(-tEchoSpacing * QR1.array());
    Eigen::ArrayXd expA = exp(-t_a * QR1.array());
    Eigen::ArrayXd expB = exp(-t_b * QR1.array());
    Eigen::ArrayXd expC = exp(-t_c * QR1.array());
    Eigen::ArrayXd expBurst = exp(-TRmp2rage * QR1.array());

    Eigen::ArrayXd cosFA1 = cos(FA1_rad * B1REL.array());
    Eigen::ArrayXd cosFA2 = cos(FA2_rad * B1REL.array());
    Eigen::ArrayXd sinFA1 = sin(FA1_rad * B1REL.array());
    Eigen::ArrayXd sinFA2 = sin(FA2_rad * B1REL.array());

    auto matrix_expEcho = onesB1.matrix() * expEcho.matrix().transpose();
    auto matrix_expA = onesB1.matrix() * expA.matrix().transpose();
    auto matrix_expB = onesB1.matrix() * expB.matrix().transpose();
    auto matrix_expC = onesB1.matrix() * expC.matrix().transpose();
    auto matrix_expBurst = onesB1.matrix() * expBurst.matrix().transpose();

    auto matrix_cosFA1 = cosFA1.matrix() * onesT1.matrix().transpose();
    auto matrix_cosFA2 = cosFA2.matrix() * onesT1.matrix().transpose();
    auto matrix_sinFA1 = sinFA1.matrix() * onesT1.matrix().transpose();
    auto matrix_sinFA2 = sinFA2.matrix() * onesT1.matrix().transpose();

    auto matrix_expEchoCosFA1 = cosFA1.matrix() * expEcho.matrix().transpose();
    auto matrix_expEchoCosFA2 = cosFA2.matrix() * expEcho.matrix().transpose();

    // Steady state FLASH signal
    // auto matrix_tmp_1 = (1 - matrix_expEchoCosFA1.array().pow(nTotal)) / (1 - matrix_expEchoCosFA1.array());
    // auto matrix_tmp_2 = (1 - matrix_expEchoCosFA2.array().pow(nTotal)) / (1 - matrix_expEchoCosFA2.array());

    // Signal in the middle of first readout -> only nBefore
    auto Mz_tmp = (1 - (inversionEfficiency * ((((((((1 - matrix_expA.array()) * matrix_expEchoCosFA1.array().pow(nTotal) + (1 - matrix_expEcho.array()) * (1 - matrix_expEchoCosFA1.array().pow(nTotal)) / (1 - matrix_expEchoCosFA1.array())) - 1) * matrix_expB.array() + 1) * matrix_expEchoCosFA2.array().pow(nTotal) + (1 - matrix_expEcho.array()) * (1 - matrix_expEchoCosFA2.array().pow(nTotal)) / (1 - matrix_expEchoCosFA2.array())) * M0 - 1) * matrix_expC.array() + 1) / (1 + inversionEfficiency * matrix_cosFA1.cwiseProduct(matrix_cosFA2).array().pow(nTotal) * matrix_expBurst.array())) / M0 + 1) * matrix_expA.array()) * matrix_expEchoCosFA1.array().pow(nBefore) + (1 - matrix_expEcho.array()) * (1 - matrix_expEchoCosFA1.array().pow(nBefore)) / (1 - matrix_expEchoCosFA1.array());
    auto signal_GRE_t_inversion1 = receptorGain * matrix_sinFA1.array() * Mz_tmp;

    // signal in the middle of first readout -> only nAfter
    auto signal_GRE_t_inversion2 = receptorGain * matrix_sinFA2.array() * ((((Mz_tmp * matrix_expEchoCosFA1.array().pow(nAfter) + (1 - matrix_expEcho.array()) * (1 - matrix_expEchoCosFA1.array().pow(nAfter)) / (1 - matrix_expEchoCosFA1.array())) - 1) * matrix_expB.array() + 1) * matrix_expEchoCosFA2.array().pow(nBefore) + (1 - matrix_expEcho.array()) * (1 - matrix_expEchoCosFA2.array().pow(nBefore)) / (1 - matrix_expEchoCosFA2.array()));

    // resulting UNI signal in unit cube ([-.5, .5])
    return (T3) (signal_GRE_t_inversion1.array() * signal_GRE_t_inversion2.array() / (signal_GRE_t_inversion1.array().square() + signal_GRE_t_inversion2.array().square()));
}

/**
 * @brief 
 * 
 * @tparam T1 
 * @tparam T2 
 * @tparam T3 
 * @param B1REL 
 * @param QT1 
 * @param tInversion1 
 * @param tInversion2 
 * @param TRmp2rage 
 * @param tEchoSpacing 
 * @param nBefore 
 * @param nAfter 
 * @param FA1_deg 
 * @param FA2_deg 
 * @param inversionEfficiency 
 * @param M0 
 * @param verbose 
 * @return T3 
 */
template <typename T1, typename T2, typename T3>
static T3 MP2RAGE_FORLOOP(const T1 &B1REL, const T2 &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, int nThreads = 1, bool verbose = true)
{

    size_t b1RelSize = B1REL.size();
    size_t qT1Size = QT1.size();
    boost::timer::auto_cpu_timer timer;

    if (verbose)
    {
        double b1step = b1RelSize > 1 ? B1REL(1) - B1REL(0) : 0;
        double qt1step = qT1Size > 1 ? QT1(1) - QT1(0) : 0;

        std::cout << "Computing theoretical T1-weighted MP2RAGE UNI signal" << "\n"
                  << "For-loop version" << "\n"
                  << "B1REL range: [" << B1REL.array().minCoeff() << ", " << B1REL.array().maxCoeff() << "], step: " << b1step << " (∅)" << "\n"
                  << "  QT1 range: [" << QT1.array().minCoeff() << ", " << QT1.array().maxCoeff() << "], step: " << qt1step << " (in provided time unit)" << "\n"
                  << "Delay to inversion #1: " << tInversion1 << " (in provided time unit)" << "\n"
                  << "Delay to inversion #2: " << tInversion2 << " (in provided time unit)" << "\n"
                  << "MP2RAGE TR           : " << TRmp2rage << " (in provided time unit)" << "\n"
                  << "Echo Spacing         : " << tEchoSpacing << " (in provided time unit)" << "\n"
                  << "# of FLASH 'before'  : " << nBefore << "\n"
                  << "# of FLASH 'after'   : " << nAfter << "\n"
                  << "FLASH Flip Angle #1  : " << FA1_deg << " (°)" << "\n"
                  << "FLASH Flip Angle #2  : " << FA2_deg << " (°)" << "\n"
                  << "Inversion Efficiency : " << inversionEfficiency << "\n"
                  << "M0                   : " << M0 << "\n"
                  << "nThreads             : " << nThreads
                  << std::endl;
    }

    assert(tInversion1 < tInversion2);
    assert(tInversion2 < TRmp2rage);
    assert((nBefore + nAfter) * tEchoSpacing < TRmp2rage);

    T3 out = T3(b1RelSize * qT1Size);

    int receptorGain = 1;
    double FA1_rad = FA1_deg * M_PI / 180;
    double FA2_rad = FA2_deg * M_PI / 180;

    int nTotal = nBefore + nAfter;
    double t_a = tInversion1 - nBefore * tEchoSpacing;
    double t_b = tInversion2 - tInversion1 - nTotal * tEchoSpacing;
    double t_c = TRmp2rage - tInversion2 - nAfter * tEchoSpacing;

    Eigen::ArrayXd QR1 = inverse(QT1.array());

    Eigen::ArrayXd expEcho = exp(-tEchoSpacing * QR1.array());
    Eigen::ArrayXd expA = exp(-t_a * QR1.array());
    Eigen::ArrayXd expB = exp(-t_b * QR1.array());
    Eigen::ArrayXd expC = exp(-t_c * QR1.array());
    Eigen::ArrayXd expBurst = exp(-TRmp2rage * QR1.array());

    Eigen::ArrayXd cosFA1 = cos(FA1_rad * B1REL.array());
    Eigen::ArrayXd cosFA2 = cos(FA2_rad * B1REL.array());
    Eigen::ArrayXd sinFA1 = sin(FA1_rad * B1REL.array());
    Eigen::ArrayXd sinFA2 = sin(FA2_rad * B1REL.array());

    #pragma omp parallel for num_threads(nThreads) schedule(guided) shared(out, expEcho, expBurst, expA, expB, expC, cosFA1, cosFA2, sinFA1, sinFA2)
    for (size_t i = 0 ; i < b1RelSize ; ++i)
    {
        double scalar_cosFA1 = cosFA1(i);
        double scalar_cosFA2 = cosFA2(i);
        double scalar_sinFA1 = sinFA1(i);
        double scalar_sinFA2 = sinFA2(i);

        for (size_t j = 0 ; j < qT1Size ; ++j)
        {
            double scalar_expEcho = expEcho(j);
            double scalar_expA = expA(j);
            double scalar_expB = expB(j);
            double scalar_expC = expC(j);
            double scalar_expBurst = expBurst(j);
            double scalar_expEchoCosFA1 = cosFA1(i) * expEcho(j);
            double scalar_expEchoCosFA2 = cosFA2(i) * expEcho(j);

            // Signal in the middle of first readout -> only nBefore
            auto Mz_tmp = (1 - (inversionEfficiency * ((((((((1 - scalar_expA) * pow(scalar_expEchoCosFA1, nTotal) + (1 - scalar_expEcho) * (1 - pow(scalar_expEchoCosFA1, nTotal)) / (1 - scalar_expEchoCosFA1)) - 1) * scalar_expB + 1) * pow(scalar_expEchoCosFA2, nTotal) + (1 - scalar_expEcho) * (1 - pow(scalar_expEchoCosFA2, nTotal)) / (1 - scalar_expEchoCosFA2)) * M0 - 1) * scalar_expC + 1) / (1 + pow(inversionEfficiency * scalar_cosFA1 * scalar_cosFA2, nTotal) * scalar_expBurst)) / M0 + 1) * scalar_expA) * pow(scalar_expEchoCosFA1, nBefore) + (1 - scalar_expEcho) * (1 - pow(scalar_expEchoCosFA1, nBefore)) / (1 - scalar_expEchoCosFA1);
            auto signal_GRE_t_inversion1 = receptorGain * scalar_sinFA1 * Mz_tmp;

            // signal in the middle of first readout -> only nAfter
            auto signal_GRE_t_inversion2 = receptorGain * scalar_sinFA2 * ((((Mz_tmp * pow(scalar_expEchoCosFA1, nAfter) + (1 - scalar_expEcho) * (1 - pow(scalar_expEchoCosFA1, nAfter)) / (1 - scalar_expEchoCosFA1)) - 1) * scalar_expB + 1) * pow(scalar_expEchoCosFA2, nBefore) + (1 - scalar_expEcho) * (1 - pow(scalar_expEchoCosFA2, nBefore)) / (1 - scalar_expEchoCosFA2));

            // resulting UNI signal in unit cube ([-.5, .5])
            out(i*b1RelSize + j) = signal_GRE_t_inversion1 * signal_GRE_t_inversion2 / (signal_GRE_t_inversion1*signal_GRE_t_inversion1 + signal_GRE_t_inversion2*signal_GRE_t_inversion2);
        }
    }

    return out;
}

template <typename T1, typename T2>
static T2 MP2RAGE_B1REL_UNITY(const T1 &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, int nThreads = 1, bool verbose = true)
{

    size_t qT1Size = QT1.size();
    boost::timer::auto_cpu_timer timer;

    if (verbose)
    {
        double qt1step = qT1Size > 1 ? QT1(1) - QT1(0) : 0;

        std::cout << "Computing theoretical T1-weighted MP2RAGE UNI signal" << "\n"
                  << "B1REL unity version" << "\n"
                  << "B1REL range: [1.0, 1.0], step: 0 (∅)" << "\n"
                  << "  QT1 range: [" << QT1.array().minCoeff() << ", " << QT1.array().maxCoeff() << "], step: " << qt1step << " (in provided time unit)" << "\n"
                  << "Delay to inversion #1: " << tInversion1 << " (in provided time unit)" << "\n"
                  << "Delay to inversion #2: " << tInversion2 << " (in provided time unit)" << "\n"
                  << "MP2RAGE TR           : " << TRmp2rage << " (in provided time unit)" << "\n"
                  << "Echo Spacing         : " << tEchoSpacing << " (in provided time unit)" << "\n"
                  << "# of FLASH 'before'  : " << nBefore << "\n"
                  << "# of FLASH 'after'   : " << nAfter << "\n"
                  << "FLASH Flip Angle #1  : " << FA1_deg << " (°)" << "\n"
                  << "FLASH Flip Angle #2  : " << FA2_deg << " (°)" << "\n"
                  << "Inversion Efficiency : " << inversionEfficiency << "\n"
                  << "M0                   : " << M0 << "\n"
                  << "nThreads             : " << nThreads
                  << std::endl;
    }

    assert(tInversion1 < tInversion2);
    assert(tInversion2 < TRmp2rage);
    assert((nBefore + nAfter) * tEchoSpacing < TRmp2rage);

    T2 out = T2(qT1Size);

    int receptorGain = 1;
    double FA1_rad = FA1_deg * M_PI / 180;
    double FA2_rad = FA2_deg * M_PI / 180;

    int nTotal = nBefore + nAfter;
    double t_a = tInversion1 - nBefore * tEchoSpacing;
    double t_b = tInversion2 - tInversion1 - nTotal * tEchoSpacing;
    double t_c = TRmp2rage - tInversion2 - nAfter * tEchoSpacing;

    Eigen::ArrayXd QR1 = inverse(QT1.array());

    Eigen::ArrayXd expEcho = exp(-tEchoSpacing * QR1.array());
    Eigen::ArrayXd expA = exp(-t_a * QR1.array());
    Eigen::ArrayXd expB = exp(-t_b * QR1.array());
    Eigen::ArrayXd expC = exp(-t_c * QR1.array());
    Eigen::ArrayXd expBurst = exp(-TRmp2rage * QR1.array());

    double cosFA1 = cos(FA1_rad);
    double cosFA2 = cos(FA2_rad);
    double sinFA1 = sin(FA1_rad);
    double sinFA2 = sin(FA2_rad);

    #pragma omp parallel for num_threads(nThreads) schedule(guided) shared(out, expEcho, expBurst, expA, expB, expC)
    for (size_t j = 0 ; j < qT1Size ; ++j)
    {
        double scalar_expEcho = expEcho(j);
        double scalar_expA = expA(j);
        double scalar_expB = expB(j);
        double scalar_expC = expC(j);
        double scalar_expBurst = expBurst(j);
        double scalar_expEchoCosFA1 = cosFA1 * scalar_expEcho;
        double scalar_expEchoCosFA2 = cosFA2 * scalar_expEcho;

        // Signal in the middle of first readout -> only nBefore
        auto Mz_tmp = (1 - (inversionEfficiency * ((((((((1 - scalar_expA) * pow(scalar_expEchoCosFA1, nTotal) + (1 - scalar_expEcho) * (1 - pow(scalar_expEchoCosFA1, nTotal)) / (1 - scalar_expEchoCosFA1)) - 1) * scalar_expB + 1) * pow(scalar_expEchoCosFA2, nTotal) + (1 - scalar_expEcho) * (1 - pow(scalar_expEchoCosFA2, nTotal)) / (1 - scalar_expEchoCosFA2)) * M0 - 1) * scalar_expC + 1) / (1 + pow(inversionEfficiency * cosFA1 * cosFA2, nTotal) * scalar_expBurst)) / M0 + 1) * scalar_expA) * pow(scalar_expEchoCosFA1, nBefore) + (1 - scalar_expEcho) * (1 - pow(scalar_expEchoCosFA1, nBefore)) / (1 - scalar_expEchoCosFA1);
        auto signal_GRE_t_inversion1 = receptorGain * sinFA1 * Mz_tmp;

        // signal in the middle of first readout -> only nAfter
        auto signal_GRE_t_inversion2 = receptorGain * sinFA2 * ((((Mz_tmp * pow(scalar_expEchoCosFA1, nAfter) + (1 - scalar_expEcho) * (1 - pow(scalar_expEchoCosFA1, nAfter)) / (1 - scalar_expEchoCosFA1)) - 1) * scalar_expB + 1) * pow(scalar_expEchoCosFA2, nBefore) + (1 - scalar_expEcho) * (1 - pow(scalar_expEchoCosFA2, nBefore)) / (1 - scalar_expEchoCosFA2));

        // resulting UNI signal in unit cube ([-.5, .5])
        out(j) = signal_GRE_t_inversion1 * signal_GRE_t_inversion2 / (signal_GRE_t_inversion1*signal_GRE_t_inversion1 + signal_GRE_t_inversion2*signal_GRE_t_inversion2);
    }

    return out;
}

/*
    OVERLOADS
*/

/**
 * @brief Overload of `MP2RAGE<T1, T2, T3>(...)`
 */
template <typename T1, typename T2>
static T1 MP2RAGE(const T1 &B1REL, const T2 &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, bool verbose = true)
{
    return MP2RAGE<T1, T2, T1>(B1REL, QT1, tInversion1, tInversion2, TRmp2rage, tEchoSpacing, nBefore, nAfter, FA1_deg, FA2_deg, inversionEfficiency, M0, verbose);
}

/**
 * @brief Overload of `MP2RAGE<T1, T2, T3>(...)`
 */
template <typename T1, typename T2>
static T2 MP2RAGE(const T1 &B1REL, const T1 &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, bool verbose = true)
{
    return MP2RAGE<T1, T1, T2>(B1REL, QT1, tInversion1, tInversion2, TRmp2rage, tEchoSpacing, nBefore, nAfter, FA1_deg, FA2_deg, inversionEfficiency, M0, verbose);
}

/**
 * @brief Overload of `MP2RAGE<T1, T2, T3>(...)`
 */
template <typename T1>
static T1 MP2RAGE(const T1 &B1REL, const T1 &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, bool verbose = true)
{
    return MP2RAGE<T1, T1, T1>(B1REL, QT1, tInversion1, tInversion2, TRmp2rage, tEchoSpacing, nBefore, nAfter, FA1_deg, FA2_deg, inversionEfficiency, M0, verbose);
}

/**
 * @brief Overload of `MP2RAGE<T1, T2, T3>(...)`
 */
static Eigen::ArrayXd MP2RAGE(const Eigen::ArrayXd &B1REL, const Eigen::ArrayXd &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, bool verbose = true)
{
    return MP2RAGE<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(B1REL, QT1, tInversion1, tInversion2, TRmp2rage, tEchoSpacing, nBefore, nAfter, FA1_deg, FA2_deg, inversionEfficiency, M0, verbose);
}

/**
 * @brief Overload of `MP2RAGE_FORLOOP<T1, T2, T3>(...)`
 */
template <typename T1, typename T2>
static T1 MP2RAGE_FORLOOP(const T1 &B1REL, const T2 &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, int nThreads = 1, bool verbose = true)
{
    return MP2RAGE_FORLOOP<T1, T2, T1>(B1REL, QT1, tInversion1, tInversion2, TRmp2rage, tEchoSpacing, nBefore, nAfter, FA1_deg, FA2_deg, inversionEfficiency, M0, nThreads, verbose);
}

/**
 * @brief Overload of `MP2RAGE_FORLOOP<T1, T2, T3>(...)`
 */
template <typename T1, typename T2>
static T2 MP2RAGE_FORLOOP(const T1 &B1REL, const T1 &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, int nThreads = 1, bool verbose = true)
{
    return MP2RAGE_FORLOOP<T1, T1, T2>(B1REL, QT1, tInversion1, tInversion2, TRmp2rage, tEchoSpacing, nBefore, nAfter, FA1_deg, FA2_deg, inversionEfficiency, M0, nThreads, verbose);
}

/**
 * @brief Overload of `MP2RAGE_FORLOOP<T1, T2, T3>(...)`
 */
template <typename T1>
static T1 MP2RAGE_FORLOOP(const T1 &B1REL, const T1 &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, int nThreads = 1, bool verbose = true)
{
    return MP2RAGE_FORLOOP<T1, T1, T1>(B1REL, QT1, tInversion1, tInversion2, TRmp2rage, tEchoSpacing, nBefore, nAfter, FA1_deg, FA2_deg, inversionEfficiency, M0, nThreads, verbose);
}

/**
 * @brief Overload of `MP2RAGE_FORLOOP<T1, T2, T3>(...)`
 */
static Eigen::ArrayXd MP2RAGE_FORLOOP(const Eigen::ArrayXd &B1REL, const Eigen::ArrayXd &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, int nThreads = 1, bool verbose = true)
{
    return MP2RAGE_FORLOOP<Eigen::ArrayXd, Eigen::ArrayXd, Eigen::ArrayXd>(B1REL, QT1, tInversion1, tInversion2, TRmp2rage, tEchoSpacing, nBefore, nAfter, FA1_deg, FA2_deg, inversionEfficiency, M0, nThreads, verbose);
}

/**
 * @brief Overload of `MP2RAGE_B1REL_UNITY<T1, T2>(...)`
 */
template <typename T1>
static T1 MP2RAGE_B1REL_UNITY(const T1 &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, int nThreads = 1, bool verbose = true)
{
    return MP2RAGE_B1REL_UNITY<T1, T1>(QT1, tInversion1, tInversion2, TRmp2rage, tEchoSpacing, nBefore, nAfter, FA1_deg, FA2_deg, inversionEfficiency, M0, nThreads, verbose);
}

/**
 * @brief Overload of `MP2RAGE_B1REL_UNITY<T1, T2>(...)`
 */
static Eigen::ArrayXd MP2RAGE_B1REL_UNITY(const Eigen::ArrayXd &QT1, double tInversion1, double tInversion2, double TRmp2rage, double tEchoSpacing, int nBefore, int nAfter, double FA1_deg, double FA2_deg, double inversionEfficiency, double M0, int nThreads = 1, bool verbose = true)
{
    return MP2RAGE_B1REL_UNITY<Eigen::ArrayXd, Eigen::ArrayXd>(QT1, tInversion1, tInversion2, TRmp2rage, tEchoSpacing, nBefore, nAfter, FA1_deg, FA2_deg, inversionEfficiency, M0, nThreads, verbose);
}
