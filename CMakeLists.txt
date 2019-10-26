cmake_minimum_required(VERSION 2.8)

#project(efanna2e LANGUAGES CXX CUDA)
project(searching LANGUAGES CUDA CXX)
enable_language(CUDA)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)
#set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_50")

include_directories(${PROJECT_SOURCE_DIR}/include)

#if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
#    add_compile_options(-O0 -Wall -Wextra -pedantic -Werror)
#    message("DEBUG ON")
#endif()

#OpenMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(FATAL_ERROR "no OpenMP supprot")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
#add_definitions (-std=c++14 -lboost -march=native -DINFO)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(CUDA)

