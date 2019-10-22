set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE CPP_SOURCES *.cpp)
#file(GLOB_RECURSE CPP_SOURCES *.cpp *.cu)
file(GLOB_RECURSE CUDA_SOURCES *.cu)

set(CXX_LIBRARY ${PROJECT_NAME})
set(CUDA_LIBRARY ${PROJECT_NAME}_CUDA)


#add_library(${PROJECT_NAME} ${CPP_SOURCES})
#add_library(${PROJECT_NAME}_s STATIC ${CPP_SOURCES})
add_library(${CXX_LIBRARY} ${CPP_SOURCES})
add_library(${CUDA_LIBRARY} ${CUDA_SOURCES})

target_compile_options(${CXX_LIBRARY} PRIVATE -lboost -march=native -DINFO)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_compile_options(${CXX_LIBRARY} PRIVATE -O0 -Wall -Wextra -pedantic -Werror)
    message("DEBUG ON")
endif()
#install()