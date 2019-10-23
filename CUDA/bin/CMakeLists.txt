add_executable(profile_cuda_test ../main/profile_cuda_test.cu)
target_link_libraries(profile_cuda_test PRIVATE index_cuda)