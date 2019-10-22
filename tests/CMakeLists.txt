set(CMAKE_CXX_STANDARD 14)

set(CXX_LIBRARY ${PROJECT_NAME})
set(CUDA_LIBRARY ${PROJECT_NAME}_CUDA)

set(CXX_OPTION -lboost -march=native -DINFO)
set(CUDA_OPTION -lboost --compiler-options -march=native -DINFO)
if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CXX_OPTION ${CXX_OPTION} -O0 -Wall -Wextra -pedantic -Werror)
endif()

add_executable(test_nsg_index test_nsg_index.cpp)
target_compile_options(test_nsg_index PRIVATE ${CXX_OPTION})
target_link_libraries(test_nsg_index PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(test_nsg_search test_nsg_search.cpp)
target_compile_options(test_nsg_search PRIVATE ${CXX_OPTION})
target_link_libraries(test_nsg_search PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(test_nsg_optimized_search test_nsg_optimized_search.cpp)
target_compile_options(test_nsg_optimized_search PRIVATE ${CXX_OPTION})
target_link_libraries(test_nsg_optimized_search PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_precision profile_precision.cpp)
target_compile_options(profile_precision PRIVATE ${CXX_OPTION})
target_link_libraries(profile_precision PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_save_NN profile_save_NN.cpp)
target_compile_options(profile_save_NN PRIVATE ${CXX_OPTION})
target_link_libraries(profile_save_NN PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_change_data_vol_and_dim profile_change_data_vol_and_dim.cpp)
target_compile_options(profile_change_data_vol_and_dim PRIVATE ${CXX_OPTION})
target_link_libraries(profile_change_data_vol_and_dim PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_gen_data profile_gen_data.cpp)
target_compile_options(profile_gen_data PRIVATE ${CXX_OPTION})
target_link_libraries(profile_gen_data PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_search_shared_candidates profile_search_shared_candidates.cpp)
target_compile_options(profile_search_shared_candidates PRIVATE ${CXX_OPTION})
target_link_libraries(profile_search_shared_candidates PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_pair_queries profile_pair_queries.cpp)
target_compile_options(profile_pair_queries PRIVATE ${CXX_OPTION})
target_link_libraries(profile_pair_queries PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_same_query_set profile_same_query_set.cpp)
target_compile_options(profile_same_query_set PRIVATE ${CXX_OPTION})
target_link_libraries(profile_same_query_set PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_joint_traversal_search profile_joint_traversal_search.cpp)
target_compile_options(profile_joint_traversal_search PRIVATE ${CXX_OPTION})
target_link_libraries(profile_joint_traversal_search PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_parallel_optimized_search profile_parallel_optimized_search.cpp)
target_compile_options(profile_parallel_optimized_search PRIVATE ${CXX_OPTION})
target_link_libraries(profile_parallel_optimized_search PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_breakdown_optimized_searching profile_breakdown_optimized_searching.cpp)
target_compile_options(profile_breakdown_optimized_searching PRIVATE ${CXX_OPTION})
target_link_libraries(profile_breakdown_optimized_searching PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_batched_joint_traversal_search profile_batched_joint_traversal_search.cpp)
target_compile_options(profile_batched_joint_traversal_search PRIVATE ${CXX_OPTION})
target_link_libraries(profile_batched_joint_traversal_search PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_reversed_nsg_index profile_reversed_nsg_index.cpp)
target_compile_options(profile_reversed_nsg_index PRIVATE ${CXX_OPTION})
target_link_libraries(profile_reversed_nsg_index PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_search_shared_tops profile_search_shared_tops.cpp)
target_compile_options(profile_search_shared_tops PRIVATE ${CXX_OPTION})
target_link_libraries(profile_search_shared_tops PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_graph_degree_distribution profile_graph_degree_distribution.cpp)
target_compile_options(profile_graph_degree_distribution PRIVATE ${CXX_OPTION})
target_link_libraries(profile_graph_degree_distribution PRIVATE ${CXX_LIBRARY} -ltcmalloc)

add_executable(profile_cuda_test profile_cuda_test.cu)
target_compile_options(profile_cuda_test PRIVATE ${CUDA_OPTION})
target_link_libraries(profile_cuda_test PRIVATE ${CUDA_LIBRARY} -ltcmalloc)