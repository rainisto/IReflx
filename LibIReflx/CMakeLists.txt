cmake_minimum_required (VERSION 3.8)
project(LibMp2tStrm)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_definitions(QSIZE=100)

if (WIN32)
    set(CMAKE_INSTALL_LIBDIR $ENV{APPDATA})
endif()

aux_source_directory(. SRC_LIST)
add_library(LibIReflx STATIC ${SRC_LIST})
set_property(TARGET LibIReflx PROPERTY POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    target_link_libraries(LibIReflx PRIVATE wsock32 ws2_32)
else()
    target_link_libraries(LibIReflx	PRIVATE pthread)
    set_target_properties(LibIReflx
        PROPERTIES
        OUTPUT_NAME IReflx)
endif()

target_include_directories(LibIReflx
          PUBLIC 
          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
          "$<INSTALL_INTERFACE:include>"
          )

set(LIB_INCLUDE_FILES IStarReflextor.h CommandLineParser.h)

# install the target and create export-set
install(TARGETS LibIReflx 
    EXPORT LibIReflxTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES ${LIB_INCLUDE_FILES} DESTINATION include)

# generate and install export file
install(EXPORT LibIReflxTargets
        FILE LibIReflxTargets.cmake
        NAMESPACE ThetaStream::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/lib/cmake/LibIReflx
)