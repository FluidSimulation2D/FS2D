cmake_minimum_required (VERSION 3.14)

project(FS2D VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(CUDA)
if (CUDA_FOUND)
	add_subdirectory(FS2D_lib)
	add_subdirectory(FS2D)
	
	target_link_libraries(FS2D FS2D_lib)
else()
	message(FATAL_ERROR " CUDA not found!")
endif()

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "external/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5.1 COMPONENTS system window graphics network audio REQUIRED)
if (SFML_FOUND)
	target_link_libraries(FS2D sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
else()
	message(FATAL_ERROR " SFML 2.5.1 not found!")
endif()

set(TGUI_STATIC_LIBRARIES TRUE)
set(TGUI_DIR "external/TGUI-1.0/lib/cmake/TGUI")

find_package(TGUI 0.10 REQUIRED)
if (TGUI_FOUND)
	target_link_libraries(FS2D TGUI::TGUI)
else()
	message(FATAL_ERROR " TGUI 0.1 not found!")
endif()