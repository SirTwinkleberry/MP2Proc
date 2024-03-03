#!/usr/bin/bash

# g++ -std=c++17 -fopenmp -O -g3 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wconversion -Wno-sign-conversion -Wdouble-promotion src/main.cpp -I. -I"./externals/include/" -I"/usr/local/include/eigen3/" -L"./externals/lib" -lRNifti -lboost_timer -o bin/main && bin/main $1
# g++ -std=c++17 -fopenmp -O3 src/main.cpp -I. -I"./externals/include/" -I"/usr/local/include/eigen3/" -L"./externals/lib" -lRNifti -lboost_timer -o bin/main && bin/main $1

# g++ -std=c++17 -O -g3 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wdouble-promotion src/wip_resample.cpp -I"/usr/local/include/ITK-5.4/" -I. -I"./externals/include/" -I"/usr/local/include/eigen3/" -L"./externals/lib" -lRNifti -lboost_timer -o bin/wip_resample && bin/wip_resample
# g++ -std=c++17 -O -g3 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wdouble-promotion src/popentest.cpp -o bin/popentest && bin/popentest


# g++ -std=c++17 -O \
#     -g3 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wdouble-promotion \
#     src/ITKtest.cpp \
#     -I"/home/tanderson/tanderson/ants/build/staging/include/ITK-5.3/" -I. -I"./externals/include/" -I"/usr/local/include/eigen3/" \
#     -L"./externals/lib" -L"/home/tanderson/tanderson/ants/install/lib/" -L"/home/tanderson/tanderson/ants/build/staging/lib/" \
#     -lRNifti -lboost_timer -pthread -lstdc++ -Wl,--no-as-needed -ldl \
#     -ll_ConvertImage \
#     -ll_CopyImageHeaderInformation \
#     -ll_WarpImageMultiTransform \
#     -ll_MultiplyImages \
#     -ll_AverageTensorImages \
#     -ll_ConvertInputImagePixelTypeToFloat \
#     -ll_ConvertToJpg \
#     -ll_MemoryTest \
#     -ll_SetSpacing \
#     -ll_antsMotionCorr \
#     -ll_LaplacianThickness \
#     -ll_LabelOverlapMeasures \
#     -ll_PermuteFlipImageOrientationAxes \
#     -ll_SurfaceCurvature \
#     -ll_ANTSUseDeformationFieldToGetAffineTransform \
#     -ll_ClusterImageStatistics \
#     -ll_CreateWarpedGridImage \
#     -ll_KellyKapowski \
#     -ll_SimulateDisplacementField \
#     -ll_ConvertScalarImageToRGB \
#     -ll_antsMotionCorrStats \
#     -ll_ANTSUseLandmarkImagesToGetAffineTransform \
#     -ll_antsRegistration \
#     -ll_SetOrigin \
#     -ll_ImageCompare \
#     -ll_ComposeMultiTransform \
#     -ll_AverageAffineTransformNoRigid \
#     -ll_ImageSetStatistics \
#     -ll_antsMotionCorrDiffusionDirection \
#     -ll_ResampleImageBySpacing \
#     -ll_FitBSplineToPoints \
#     -ll_simpleSynRegistration \
#     -ll_N4BiasFieldCorrection \
#     -ll_ConvertImagePixelType \
#     -ll_sccan \
#     -ll_antsJointFusion \
#     -ll_ExtractSliceFromImage \
#     -ll_AddNoiseToImage \
#     -ll_LabelGeometryMeasures \
#     -ll_antsAffineInitializer \
#     -ll_antsApplyTransforms \
#     -ll_MeasureImageSimilarity \
#     -ll_SurfaceBasedSmoothing \
#     -ll_ANTS \
#     -ll_SmoothDisplacementField \
#     -ll_ImageMath \
#     -ll_N3BiasFieldCorrection \
#     -ll_SetDirectionByMatrix \
#     -ll_antsJointTensorFusion \
#     -ll_ANTSIntegrateVectorField \
#     -ll_PrintHeader \
#     -ll_ResampleImage \
#     -ll_PasteImageIntoImage \
#     -ll_SmoothImage \
#     -ll_antsAI \
#     -ll_ThresholdImage \
#     -ll_Atropos \
#     -ll_iMath \
#     -ll_ANTSUseLandmarkImagesToGetBSplineDisplacementField \
#     -ll_LabelClustersUniquely \
#     -ll_antsApplyTransformsToPoints \
#     -ll_LesionFilling \
#     -ll_DenoiseImage \
#     -ll_antsLandmarkBasedTransformInitializer \
#     -ll_ANTSIntegrateVelocityField \
#     -ll_CreateJacobianDeterminantImage \
#     -ll_MeasureMinMaxMean \
#     -ll_ResetDirection \
#     -ll_WarpTimeSeriesImageMultiTransform \
#     -ll_ANTSJacobian \
#     -ll_ExtractRegionFromImage \
#     -ll_ReorientTensorImage \
#     -ll_ExtractRegionFromImageByMask \
#     -ll_NonLocalSuperResolution \
#     -ll_TileImages \
#     -ll_CreateTiledMosaic \
#     -ll_CreateImage \
#     -ll_KellySlater \
#     -ll_WarpTensorImageMultiTransform \
#     -ll_CreateDTICohort \
#     -ll_antsSliceRegularizedRegistration \
#     -ll_RebaseTensorImage \
#     -ll_antsAlignOrigin \
#     -ll_AverageImages \
#     -ll_antsUtilitiesTesting \
#     -ll_antsTransformInfo \
#     -ll_AverageAffineTransform \
#     -ll_CompositeTransformUtil \
#     -ll_ConvertTransformFile \
#     -ll_CreateDisplacementField \
#     -ll_SuperResolution \
#     -ll_compareTwoTransforms \
#     -ll_TimeSCCAN \
#     -ll_TextureCooccurrenceFeatures \
#     -ll_GetConnectedComponentsFeatureImages \
#     -ll_TextureRunLengthFeatures \
#     -ll_ImageIntensityStatistics \
#     -ll_StackSlices \
#     -ll_DeNrrd \
#     -lantsUtilities \
#     -lITKReview-5.3 \
#     -lITKTestKernel-5.3 \
#     -lITKPDEDeformableRegistration-5.3 \
#     -lITKDeformableMesh-5.3 \
#     -lITKRegistrationMethodsv4-5.3 \
#     -lITKIOSpatialObjects-5.3 \
#     -lITKWatersheds-5.3 \
#     -lITKIOMeshVTK-5.3 \
#     -lITKIOMeshFreeSurfer-5.3 \
#     -lITKIOMeshOFF-5.3 \
#     -lITKIOMeshGifti-5.3 \
#     -lITKIOMeshOBJ-5.3 \
#     -lITKIOMeshBYU-5.3 \
#     -lITKLabelMap-5.3 \
#     -lITKImageFeature-5.3 \
#     -lITKBiasCorrection-5.3 \
#     -lITKMathematicalMorphology-5.3 \
#     -litkAdaptiveDenoising-5.3 \
#     -lITKConvolution-5.3 \
#     -lITKMarkovRandomFieldsClassifiers-5.3 \
#     -lITKRegionGrowing-5.3 \
#     -lITKIOMeshBase-5.3 \
#     -lITKIOTransformMatlab-5.3 \
#     -lITKSpatialObjects-5.3 \
#     -lITKIOTransformHDF5-5.3 \
#     -lITKIOTransformInsightLegacy-5.3 \
#     -lITKQuadEdgeMesh-5.3 \
#     -lITKIOTransformBase-5.3 \
#     -lITKVideoIO-5.3 \
#     -lITKQuadEdgeMeshFiltering-5.3 \
#     -lITKIOLSM-5.3 \
#     -lITKIOGE-5.3 \
#     -lITKMesh-5.3 \
#     -lITKOptimizersv4-5.3 \
#     -lITKTransformFactory-5.3 \
#     -litkMGHIO-5.3 \
#     -lITKVideoCore-5.3 \
#     -lITKKLMRegionGrowing-5.3 \
#     -lITKVTK-5.3 \
#     -lITKIOXML-5.3 \
#     -lITKIOVTK-5.3 \
#     -lITKIOStimulate-5.3 \
#     -lITKIOSiemens-5.3 \
#     -lITKIONRRD-5.3 \
#     -lITKIOPNG-5.3 \
#     -lITKIONIFTI-5.3 \
#     -lITKIOMeta-5.3 \
#     -lITKIOMRC-5.3 \
#     -lITKIOMINC-5.3 \
#     -lITKIOTIFF-5.3 \
#     -lITKIOJPEG-5.3 \
#     -lITKIOJPEG2000-5.3 \
#     -lITKIOHDF5-5.3 \
#     -lITKOptimizers-5.3 \
#     -lITKIOGDCM-5.3 \
#     -lITKIOGIPL-5.3 \
#     -lITKIOIPL-5.3 \
#     -lITKIOBruker-5.3 \
#     -lITKIOCSV-5.3 \
#     -lITKIOBioRad-5.3 \
#     -lITKIOBMP-5.3 \
#     -lITKPolynomials-5.3 \
#     -lITKIOImageBase-5.3 \
#     -litkminc2-5.3 \
#     -lITKStatistics-5.3 \
#     -lITKFFT-5.3 \
#     -lITKPath-5.3 \
#     -lITKTransform-5.3 \
#     -litkgdcmMEXD-5.3 \
#     -lITKCommon-5.3 \
#     -lhdf5_hl_cpp-static \
#     -litkhdf5_hl-static-5.3 \
#     -litkhdf5_cpp-static-5.3 \
#     -litkhdf5-static-5.3 \
#     -litkgdcmMSFF-5.3 \
#     -lITKVNLInstantiation-5.3 \
#     -litkvnl_algo-5.3 \
#     -litkNetlibSlatec-5.3 \
#     -litkv3p_netlib-5.3 \
#     -litkgdcmDICT-5.3 \
#     -litkgdcmIOD-5.3 \
#     -litkvnl-5.3 \
#     -litkgdcmDSED-5.3 \
#     -lITKgiftiio-5.3 \
#     -litktiff-5.3 \
#     -lITKniftiio-5.3 \
#     -lITKMetaIO-5.3 \
#     -lITKNrrdIO-5.3 \
#     -litktestlib-5.3 \
#     -litklbfgs-5.3 \
#     -litkpng-5.3 \
#     -litkjpeg-5.3 \
#     -litkopenjpeg-5.3 \
#     -litkgdcmsocketxx-5.3 \
#     -lITKznz-5.3 \
#     -litkgdcmopenjp2-5.3 \
#     -litkgdcmuuid-5.3 \
#     -litkgdcmcharls-5.3 \
#     -litkgdcmjpeg16-5.3 \
#     -litkgdcmjpeg8-5.3 \
#     -litkgdcmjpeg12-5.3 \
#     -litkgdcmCommon-5.3 \
#     -lITKFastMarching-5.3 \
#     -lITKEXPAT-5.3 \
#     -lITKDenoising-5.3 \
#     -lITKDiffusionTensorImage-5.3 \
#     -lITKDICOMParser-5.3 \
#     -lITKSmoothing-5.3 \
#     -lITKColormap-5.3 \
#     -litkzlib-5.3 \
#     -lITKImageIntensity-5.3 \
#     -litksys-5.3 \
#     -litkdouble-conversion-5.3 \
#     -litkvcl-5.3 \
#     -o bin/ITKtest \
#     && bin/ITKtest

# g++ -std=c++17 \
#     src/ITKtest.cpp \
#     -I"/home/tanderson/tanderson/ants/build/staging/include/ITK-5.3/" \
#     -L"/home/tanderson/tanderson/ants/install/lib/" -L"/home/tanderson/tanderson/ants/build/staging/lib/" \
#     -pthread -lstdc++ -Wl,--no-as-needed -ldl \
#     -ll_antsApplyTransforms \
#     -ll_SmoothImage \
#     -lantsUtilities \
#     -lITKIOTransformMatlab-5.3 \
#     -lITKIOTransformHDF5-5.3 \
#     -lITKIOTransformInsightLegacy-5.3 \
#     -lITKIOTransformBase-5.3 \
#     -lITKIOLSM-5.3 \
#     -lITKIOGE-5.3 \
#     -lITKTransformFactory-5.3 \
#     -litkMGHIO-5.3 \
#     -lITKIOXML-5.3 \
#     -lITKIOVTK-5.3 \
#     -lITKIOStimulate-5.3 \
#     -lITKIOSiemens-5.3 \
#     -lITKIONRRD-5.3 \
#     -lITKIOPNG-5.3 \
#     -lITKIONIFTI-5.3 \
#     -lITKIOMeta-5.3 \
#     -lITKIOMRC-5.3 \
#     -lITKIOMINC-5.3 \
#     -lITKIOTIFF-5.3 \
#     -lITKIOJPEG-5.3 \
#     -lITKIOJPEG2000-5.3 \
#     -lITKIOHDF5-5.3 \
#     -lITKIOGDCM-5.3 \
#     -lITKIOGIPL-5.3 \
#     -lITKIOIPL-5.3 \
#     -lITKIOBruker-5.3 \
#     -lITKIOCSV-5.3 \
#     -lITKIOBioRad-5.3 \
#     -lITKIOBMP-5.3 \
#     -lITKIOImageBase-5.3 \
#     -litkminc2-5.3 \
#     -lITKTransform-5.3 \
#     -lITKCommon-5.3 \
#     -lhdf5_hl_cpp-static \
#     -litkhdf5_hl-static-5.3 \
#     -litkhdf5_cpp-static-5.3 \
#     -litkhdf5-static-5.3 \
#     -litkgdcmMSFF-5.3 \
#     -litkvnl_algo-5.3 \
#     -litkv3p_netlib-5.3 \
#     -litkgdcmDICT-5.3 \
#     -litkgdcmIOD-5.3 \
#     -litkvnl-5.3 \
#     -litkgdcmDSED-5.3 \
#     -litktiff-5.3 \
#     -lITKniftiio-5.3 \
#     -lITKMetaIO-5.3 \
#     -lITKNrrdIO-5.3 \
#     -litkpng-5.3 \
#     -litkjpeg-5.3 \
#     -litkopenjpeg-5.3 \
#     -litkgdcmsocketxx-5.3 \
#     -lITKznz-5.3 \
#     -litkgdcmopenjp2-5.3 \
#     -litkgdcmuuid-5.3 \
#     -litkgdcmcharls-5.3 \
#     -litkgdcmjpeg16-5.3 \
#     -litkgdcmjpeg8-5.3 \
#     -litkgdcmjpeg12-5.3 \
#     -litkgdcmCommon-5.3 \
#     -lITKEXPAT-5.3 \
#     -lITKSmoothing-5.3 \
#     -litkzlib-5.3 \
#     -litksys-5.3 \
#     -litkdouble-conversion-5.3 \
#     -o bin/ITKtest \
#     && bin/ITKtest


g++ -std=c++17 \
    src/ITKtest.cpp \
    -L"./externals/lib" \
    -pthread -lstdc++ -Wl,--no-as-needed -ldl \
    -ll_antsApplyTransforms \
    -ll_SmoothImage \
    -lantsUtilities \
    -lITKIOTransformMatlab-5.3 \
    -lITKIOTransformHDF5-5.3 \
    -lITKIOTransformInsightLegacy-5.3 \
    -lITKIOTransformBase-5.3 \
    -lITKIOLSM-5.3 \
    -lITKIOGE-5.3 \
    -lITKTransformFactory-5.3 \
    -litkMGHIO-5.3 \
    -lITKIOXML-5.3 \
    -lITKIOVTK-5.3 \
    -lITKIOStimulate-5.3 \
    -lITKIOSiemens-5.3 \
    -lITKIONRRD-5.3 \
    -lITKIOPNG-5.3 \
    -lITKIONIFTI-5.3 \
    -lITKIOMeta-5.3 \
    -lITKIOMRC-5.3 \
    -lITKIOMINC-5.3 \
    -lITKIOTIFF-5.3 \
    -lITKIOJPEG-5.3 \
    -lITKIOJPEG2000-5.3 \
    -lITKIOHDF5-5.3 \
    -lITKIOGDCM-5.3 \
    -lITKIOGIPL-5.3 \
    -lITKIOIPL-5.3 \
    -lITKIOBruker-5.3 \
    -lITKIOCSV-5.3 \
    -lITKIOBioRad-5.3 \
    -lITKIOBMP-5.3 \
    -lITKIOImageBase-5.3 \
    -litkminc2-5.3 \
    -lITKTransform-5.3 \
    -lITKCommon-5.3 \
    -lhdf5_hl_cpp-static \
    -litkhdf5_hl-static-5.3 \
    -litkhdf5_cpp-static-5.3 \
    -litkhdf5-static-5.3 \
    -litkgdcmMSFF-5.3 \
    -litkvnl_algo-5.3 \
    -litkv3p_netlib-5.3 \
    -litkgdcmDICT-5.3 \
    -litkgdcmIOD-5.3 \
    -litkvnl-5.3 \
    -litkgdcmDSED-5.3 \
    -litktiff-5.3 \
    -lITKniftiio-5.3 \
    -lITKMetaIO-5.3 \
    -lITKNrrdIO-5.3 \
    -litkpng-5.3 \
    -litkjpeg-5.3 \
    -litkopenjpeg-5.3 \
    -litkgdcmsocketxx-5.3 \
    -lITKznz-5.3 \
    -litkgdcmopenjp2-5.3 \
    -litkgdcmuuid-5.3 \
    -litkgdcmcharls-5.3 \
    -litkgdcmjpeg16-5.3 \
    -litkgdcmjpeg8-5.3 \
    -litkgdcmjpeg12-5.3 \
    -litkgdcmCommon-5.3 \
    -lITKEXPAT-5.3 \
    -lITKSmoothing-5.3 \
    -litkzlib-5.3 \
    -litksys-5.3 \
    -litkdouble-conversion-5.3 \
    -o bin/ITKtest \
    && bin/ITKtest