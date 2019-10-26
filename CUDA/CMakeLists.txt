set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(./include)

set(CXX_OPTION -lboost -march=native -DINFO)
set(CXX_LIBRARY -ltcmalloc)
if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CXX_OPTION ${CXX_OPTION} -O0 -Wall -Wextra -pedantic -Werror)
endif()

# CXX Library
add_library(searching_cxx
        ./src/index_nd.cpp
        ./src/index_nd_nsg.cpp)
target_compile_options(searching_cxx PRIVATE  -lboost -march=native -DINFO)

# CUDA Library
add_library(searching_cuda
        ./src/index_cuda.cu)

# Bin
add_executable(profile_cuda_test ./main/profile_cuda_test.cu)
target_link_libraries(profile_cuda_test PRIVATE searching_cuda searching_cxx)