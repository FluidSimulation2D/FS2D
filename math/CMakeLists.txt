CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

PROJECT(sum_lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/add.cuh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/sum.h
    )

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/add.cu
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sum.cpp
    )

CUDA_ADD_LIBRARY(sum_lib ${SOURCES} ${HEADERS})
