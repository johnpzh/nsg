set(CMAKE_CXX_STANDARD 14)

add_executable(test_nsg_index test_nsg_index.cpp)
target_link_libraries(test_nsg_index ${PROJECT_NAME} -ltcmalloc Boost::boost)
#target_link_libraries(test_nsg_index PRIVATE -ltcmalloc)
#target_link_libraries(test_nsg_index PRIVATE Boost::boost)

add_executable(test_nsg_search test_nsg_search.cpp)
target_link_libraries(test_nsg_search ${PROJECT_NAME} -ltcmalloc Boost::boost)
#target_link_libraries(test_nsg_search PRIVATE -ltcmalloc)
#target_link_libraries(test_nsg_search PRIVATE Boost::boost)

add_executable(test_nsg_optimized_search test_nsg_optimized_search.cpp)
target_link_libraries(test_nsg_optimized_search ${PROJECT_NAME} -ltcmalloc Boost::boost)
#target_link_libraries(test_nsg_optimized_search PRIVATE -ltcmalloc)
#target_link_libraries(test_nsg_optimized_search PRIVATE Boost::boost)

add_executable(test_precision test_precision.cpp)
target_link_libraries(test_precision ${PROJECT_NAME} -ltcmalloc Boost::boost)
#target_link_libraries(test_precision PRIVATE -ltcmalloc)
#target_link_libraries(test_precision PRIVATE Boost::boost)