set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR}/CUDA/include)

set(CXX_OPTION -lboost -march=native -DINFO)
set(CXX_LIBRARY -ltcmalloc)
if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CXX_OPTION ${CXX_OPTION} -O0 -Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(lib)
add_subdirectory(bin)