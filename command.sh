#!/usr/bin/bash

# g++ -std=c++17 -fopenmp -O -g3 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wconversion -Wno-sign-conversion -Wdouble-promotion src/main.cpp -I. -I"./externals/include/" -I"/usr/local/include/eigen3/" -L"./externals/lib" -lRNifti -lboost_timer -o bin/main && bin/main $1
# g++ -std=c++17 -fopenmp -O3 src/main.cpp -I. -I"./externals/include/" -I"/usr/local/include/eigen3/" -L"./externals/lib" -lRNifti -lboost_timer -o bin/main && bin/main $1

g++ -std=c++17 -O -g3 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wconversion -Wno-sign-conversion -Wdouble-promotion src/wip_resample.cpp -I. -I"./externals/include/" -I"/usr/local/include/eigen3/" -L"./externals/lib" -lRNifti -lboost_timer -o bin/wip_resample && bin/wip_resample
