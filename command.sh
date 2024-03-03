#!/usr/bin/bash

ANTSLIBS=(
    -pthread -lstdc++ -Wl,--no-as-needed -ldl
    -ll_antsApplyTransforms
    -ll_SmoothImage
    -lantsUtilities
)

ITKLIBS=(
    -lITKIOTransformMatlab-5.3
    -lITKIOTransformHDF5-5.3
    -lITKIOTransformInsightLegacy-5.3
    -lITKIOTransformBase-5.3
    -lITKIOLSM-5.3
    -lITKIOGE-5.3
    -lITKTransformFactory-5.3
    -litkMGHIO-5.3
    -lITKIOXML-5.3
    -lITKIOVTK-5.3
    -lITKIOStimulate-5.3
    -lITKIOSiemens-5.3
    -lITKIONRRD-5.3
    -lITKIOPNG-5.3
    -lITKIONIFTI-5.3
    -lITKIOMeta-5.3
    -lITKIOMRC-5.3
    -lITKIOMINC-5.3
    -lITKIOTIFF-5.3
    -lITKIOJPEG-5.3
    -lITKIOJPEG2000-5.3
    -lITKIOHDF5-5.3
    -lITKIOGDCM-5.3
    -lITKIOGIPL-5.3
    -lITKIOIPL-5.3
    -lITKIOBruker-5.3
    -lITKIOCSV-5.3
    -lITKIOBioRad-5.3
    -lITKIOBMP-5.3
    -lITKIOImageBase-5.3
    -litkminc2-5.3
    -lITKTransform-5.3
    -lITKCommon-5.3
    -lhdf5_hl_cpp-static
    -litkhdf5_hl-static-5.3
    -litkhdf5_cpp-static-5.3
    -litkhdf5-static-5.3
    -litkgdcmMSFF-5.3
    -litkvnl_algo-5.3
    -litkv3p_netlib-5.3
    -litkgdcmDICT-5.3
    -litkgdcmIOD-5.3
    -litkvnl-5.3
    -litkgdcmDSED-5.3
    -litktiff-5.3
    -lITKniftiio-5.3
    -lITKMetaIO-5.3
    -lITKNrrdIO-5.3
    -litkpng-5.3
    -litkjpeg-5.3
    -litkopenjpeg-5.3
    -litkgdcmsocketxx-5.3
    -lITKznz-5.3
    -litkgdcmopenjp2-5.3
    -litkgdcmuuid-5.3
    -litkgdcmcharls-5.3
    -litkgdcmjpeg16-5.3
    -litkgdcmjpeg8-5.3
    -litkgdcmjpeg12-5.3
    -litkgdcmCommon-5.3
    -lITKEXPAT-5.3
    -lITKSmoothing-5.3
    -litkzlib-5.3
    -litksys-5.3
    -litkdouble-conversion-5.3
)

CXXFLAGS="-std=c++17"

if [ "$2" = "debug" ]; then
    CPPFLAGS="-fopenmp -O -g3 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wconversion -Wno-sign-conversion -Wdouble-promotion ${@:5}"
else
    CPPFLAGS="-fopenmp -O3 ${@:5}"
fi
echo ">> $2 compilation <<"

LIBS="-L./externals/lib -lboost_timer ${ANTSLIBS[@]} ${ITKLIBS[@]}"
INCLUDE="-I./externals/include"
SRC="./src"
BIN="./bin"
PROGRAM=main

echo "g++ ${CXXFLAGS} ${CPPFLAGS}" \
    "${INCLUDE}" \
    "${SRC}/${PROGRAM}.cpp" \
    "${LIBS}" \
    "-o ${BIN}/${PROGRAM}" \
    "&& ${BIN}/${PROGRAM} $1"

g++ ${CXXFLAGS} ${CPPFLAGS} \
    ${INCLUDE} \
    ${SRC}/${PROGRAM}.cpp \
    ${LIBS} \
    -o ${BIN}/${PROGRAM} \
    && ${BIN}/${PROGRAM} $1

# -Wno-deprecated-declarations to remove deprecation warnings
