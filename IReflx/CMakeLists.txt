# CMakeList.txt : CMake project for IReflx, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(IReflx)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (WIN32)
    set(CMAKE_INSTALL_LIBDIR $ENV{APPDATA})
else()
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

# Add source to this project's executable.
add_executable (IReflx "main.cpp" )

list(APPEND EXTRA_LIBS LibIReflx)

target_link_libraries(IReflx PUBLIC ${EXTRA_LIBS})

# Install targets
install(TARGETS IReflx 
    EXPORT IReflxTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
