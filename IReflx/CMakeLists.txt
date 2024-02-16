cmake_minimum_required (VERSION 3.21)

project(IReflx
	VERSION 1.0.0
	DESCRIPTION "ISTAR Reflextor Library"
	LANGUAGES C CXX
)

# set the postfix "d" for the resulting .so or .dll files when building the
# library in debug mode
set(CMAKE_DEBUG_POSTFIX d)

add_library(IReflx STATIC)

# specify the C++ standard
target_compile_features(
    IReflx
    PUBLIC cxx_std_14
)

add_compile_definitions(QSIZE=100)

file(GLOB SRC_LIST
	src/*.cpp)

target_sources(IReflx PRIVATE ${SRC_LIST})

set_property(TARGET IReflx PROPERTY POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    set(WS wsock32 ws2_32)
endif()

target_link_libraries(IReflx ${WS})

target_include_directories(IReflx
		PRIVATE src/IReflx
		PUBLIC include
)