cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(efanna2e)

include_directories(${PROJECT_SOURCE_DIR}/include)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_compile_options(-O0 -Wall -Wextra -pedantic -Werror)
    message("DEBUG ON")
endif()

#OpenMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(FATAL_ERROR "no OpenMP supprot")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
#add_definitions (-std=c++11 -O3 -lboost -march=native -Wall -DINFO)
add_definitions(-march=native -DINFO)

# Boost
find_package(Boost REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)


