cmake_minimum_required(VERSION 3.18)

project(FS2D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../FS2D_lib/include)
include_directories("${CUDA_INCLUDE_DIRS}")

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/window.h
    )

set(SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/window.cpp
    )

add_executable(FS2D ${SOURCES} ${HEADERS})