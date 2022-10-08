cmake_minimum_required (VERSION 3.14)

project(Fluid_Simulation VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(CUDA)
if (CUDA_FOUND)
	add_subdirectory(fluid_simulation)
	add_subdirectory(math)
	
	target_link_libraries(run sum_lib)
else()
	message(FATAL_ERROR " CUDA not found!")
endif()


