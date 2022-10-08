cmake_minimum_required(VERSION 3.18)

project(run LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../math/include)

set(HEADERS
    )

set(SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    )

add_executable(run ${SOURCES} ${HEADERS})