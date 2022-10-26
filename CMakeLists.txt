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

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "external/SFML/lib/cmake/SFML")

find_package(SFML 2.5.1 COMPONENTS system window graphics network audio REQUIRED)
if (SFML_FOUND)
	target_link_libraries(run sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
else()
	message(FATAL_ERROR " SFML 2.5.1 not found!")
endif()

set(TGUI_STATIC_LIBRARIES TRUE)
set(TGUI_DIR "external/TGUI/lib/cmake/TGUI")

find_package(TGUI 0.10 REQUIRED)
if (TGUI_FOUND)
	target_link_libraries(run TGUI::TGUI)
else()
	message(FATAL_ERROR " TGUI 0.1 not found!")
endif()