//
// Created by Zhen Peng on 10/22/19.
//

//#include <efanna2e/index_nsg.h>
//#include <efanna2e/util.h>
//#include <chrono>
//#include <string>
//// Added by Johnpzh
//#include <omp.h>
//#include  "extras/utils.h"
#include <stdio.h>
#include <iostream>
#include "../include/index_nd_nsg.h"
#include "../include/index_cuda.cuh"
//// Ended by Johnpzh

void load_data(char *filename, float *&data, unsigned &num,
               unsigned &dim)
{  // load data with sift10K pattern
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    in.read((char *) &dim, 4);
    // std::cout<<"data dimension: "<<dim<<std::endl;
    in.seekg(0, std::ios::end);
    std::ios::pos_type ss = in.tellg();
    size_t fsize = (size_t) ss;
    num = (unsigned) (fsize / (dim + 1) / 4);
    data = new float[(size_t) num * (size_t) dim];

    in.seekg(0, std::ios::beg);
    for (size_t i = 0; i < num; i++) {
        in.seekg(4, std::ios::cur);
        in.read((char *) (data + i * dim), dim * 4);
    }
    in.close();
}

int main(int argc, char **argv)
{

    if (argc != 8) {
        std::cout << argv[0]
                  << " data_file query_file nsg_path search_L search_K result_path query_num_max"
                  << std::endl;
        exit(-1);
    }
    setbuf(stdout, nullptr); // Added by Johnpzh
    float *data_load = NULL;
    unsigned points_num, dim;
    load_data(argv[1], data_load, points_num, dim);
    float *query_load = NULL;
    unsigned query_num, query_dim;
    load_data(argv[2], query_load, query_num, query_dim);
    assert(dim == query_dim);
    // Added by Johnpzh
    {
        unsigned query_num_max = strtoull(argv[7], nullptr, 0);
        if (query_num > query_num_max) query_num = query_num_max;
//        printf("query_num: %u\n", query_num);
    }
    // Ended by Johnpzh

    unsigned L = (unsigned) atoi(argv[4]);
    unsigned K = (unsigned) atoi(argv[5]);

    if (L < K) {
        std::cout << "search_L cannot be smaller than search_K!" << std::endl;
        exit(-1);
    }

    // data_load = efanna2e::data_align(data_load, points_num, dim);//one must
    // align the data before build query_load = efanna2e::data_align(query_load,
    // query_num, query_dim);
    efanna2e::IndexNDNSG index(dim, points_num, nullptr);
    index.Load(argv[3]);
    index.OptimizeGraph(data_load);

    bool *d_switch;
    cudaMalloc(&d_switch, sizeof(bool));
    efanna2e::cudaPrint<<<2, 2>>>(d_switch);
    bool h_switch;
    cudaMemcpy(&h_switch, d_switch, sizeof(bool), cudaMemcpyDeviceToHost);
    cudaFree(d_switch);
    if (h_switch) {
        printf("Switched.\n");
    }

    return 0;
}
