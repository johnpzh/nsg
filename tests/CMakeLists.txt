set(CMAKE_CXX_STANDARD 14)

add_executable(test_nsg_index test_nsg_index.cpp)
target_link_libraries(test_nsg_index ${PROJECT_NAME} -ltcmalloc)

add_executable(test_nsg_search test_nsg_search.cpp)
target_link_libraries(test_nsg_search ${PROJECT_NAME})

add_executable(test_nsg_optimized_search test_nsg_optimized_search.cpp)
target_link_libraries(test_nsg_optimized_search ${PROJECT_NAME} -ltcmalloc)

add_executable(profile_precision profile_precision.cpp)
target_link_libraries(profile_precision ${PROJECT_NAME} -ltcmalloc)

add_executable(profile_save_NN profile_save_NN.cpp)
target_link_libraries(profile_save_NN ${PROJECT_NAME} -ltcmalloc)

add_executable(profile_change_data_vol_and_dim profile_change_data_vol_and_dim.cpp)
target_link_libraries(profile_change_data_vol_and_dim ${PROJECT_NAME} -ltcmalloc)

add_executable(profile_gen_data profile_gen_data.cpp)
target_link_libraries(profile_gen_data ${PROJECT_NAME} -ltcmalloc)

add_executable(profile_search_shared_candidates profile_search_shared_candidates.cpp)
target_link_libraries(profile_search_shared_candidates ${PROJECT_NAME} -ltcmalloc)

add_executable(profile_pair_queries profile_pair_queries.cpp)
target_link_libraries(profile_pair_queries ${PROJECT_NAME} -ltcmalloc)

add_executable(profile_same_query_set profile_same_query_set.cpp)
target_link_libraries(profile_same_query_set ${PROJECT_NAME} -ltcmalloc)

add_executable(profile_joint_traversal_search profile_joint_traversal_search.cpp)
target_link_libraries(profile_joint_traversal_search ${PROJECT_NAME} -ltcmalloc)

add_executable(profile_parallel_optimized_search profile_parallel_optimized_search.cpp)
target_link_libraries(profile_parallel_optimized_search ${PROJECT_NAME} -ltcmalloc)

add_executable(profile_breakdown_optimized_searching profile_breakdown_optimized_searching.cpp)
target_link_libraries(profile_breakdown_optimized_searching ${PROJECT_NAME} -ltcmalloc)