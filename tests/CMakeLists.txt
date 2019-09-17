set(CMAKE_CXX_STANDARD 14)

add_executable(test_nsg_index test_nsg_index.cpp)
target_link_libraries(test_nsg_index ${PROJECT_NAME} -ltcmalloc)

add_executable(test_nsg_search test_nsg_search.cpp)
target_link_libraries(test_nsg_search ${PROJECT_NAME})

add_executable(test_nsg_optimized_search test_nsg_optimized_search.cpp)
target_link_libraries(test_nsg_optimized_search ${PROJECT_NAME} -ltcmalloc)

add_executable(test_precision test_precision.cpp)
target_link_libraries(test_precision ${PROJECT_NAME} -ltcmalloc)

add_executable(test_save_NN test_save_NN.cpp)
target_link_libraries(test_save_NN ${PROJECT_NAME} -ltcmalloc)

add_executable(test_change_data_vol_and_dim test_change_data_vol_and_dim.cpp)
target_link_libraries(test_change_data_vol_and_dim ${PROJECT_NAME} -ltcmalloc)

add_executable(test_gen_data test_gen_data.cpp)
target_link_libraries(test_gen_data ${PROJECT_NAME} -ltcmalloc)