# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)

project (IReflx
	VERSION 1.0.0
	DESCRIPTION "The ISTAR Reflextor application."
	LANGUAGES C CXX)
	
include(GNUInstallDirs)

# Include sub-projects.
add_subdirectory ("IReflx")

add_executable(IReflxApp)

target_sources(IReflxApp PRIVATE src/main.cpp)

target_link_libraries(IReflxApp PRIVATE IReflx)

install (TARGETS IReflxApp)

# Test cases
