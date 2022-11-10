CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

PROJECT(FS2D_lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories("${CUDA_INCLUDE_DIRS}")

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/fluid.h
	)

set(SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/fluid_cuda.cu
	)

CUDA_ADD_LIBRARY(FS2D_lib ${SOURCES} ${HEADERS})
