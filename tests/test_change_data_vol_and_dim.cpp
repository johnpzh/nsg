//
// Created by Zhen Peng on 9/16/19.
//
/*
 * Change the volume and the dimension of the input data and save to output_data.
 * Also change the dimension of the query file and then save to output_query.
 */
#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <chrono>
#include <string>
#include <omp.h>
#include  "extras/utils.h"
//#include <math.h>

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

// Function: save output file according to output_volume and output_dimension.
void save_output_file(
        const char *filename,
        const float *data,
        unsigned output_vol,
        unsigned output_dim,
        unsigned input_dim)
{
    std::ofstream fout(filename);
    if (!fout.is_open()) {
        fprintf(stderr, "Error: cannot create file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fout.write(reinterpret_cast<char *>(&output_dim), sizeof(output_dim));
//    size_t data_index = 0;
    uint32_t blank = 0;
    for (unsigned i = 0; i < output_vol; ++i) {
        fout.write(reinterpret_cast<const char *>(&blank), 4);
        fout.write(reinterpret_cast<const char *>(data + i * input_dim), output_dim * 4);
    }
    fout.close();
}

int main(int argc, char **argv)
{
    if (argc != 7) {
        std::cout << argv[0]
                  << " data_file query_file output_volume output_dimension output_data_file output_query_file"
//                  << " data_file query_file nsg_path search_L search_K true-NN_path query_num_max"
                  << std::endl;
        exit(-1);
    }
    setbuf(stdout, nullptr); // Added by Johnpzh
    float *data_load = nullptr;
    unsigned points_num, dim;
    load_data(argv[1], data_load, points_num, dim);
    float *query_load = nullptr;
    unsigned query_num, query_dim;
    load_data(argv[2], query_load, query_num, query_dim);
    assert(dim == query_dim);

    unsigned output_volume = strtoull(argv[3], nullptr, 0);
    unsigned output_dimension = strtoull(argv[4], nullptr, 0);
    if (points_num < output_volume) {
        fprintf(stderr, "Error: output_volume %u is larger than %u the volume of input data.\n",
                output_volume, points_num);
        exit(EXIT_FAILURE);
    } else if (dim < output_dimension) {
        fprintf(stderr, "Error: output_dimension %u is larger than %u the dimension of input data.\n",
                output_dimension, dim);
        exit(EXIT_FAILURE);
    }
    auto s = std::chrono::high_resolution_clock::now();

    // Change volume of data and save to the output_data
    save_output_file(
            argv[5],
            data_load,
            output_volume,
            output_dimension,
            dim);

    // Change dimension of data and query and save to output_data and output_query
    save_output_file(
            argv[6],
            query_load,
            query_num,
            output_dimension,
            dim);

    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = e - s;
    printf("volume: %u output_volume: %u dim: %u output_dim: %u\n",
            points_num, output_volume, dim, output_dimension);
    std::cout << "run_time: " << diff.count() << "\n";

//    save_result(argv[6], res);
    delete [] data_load;
    delete [] query_load;
    return 0;
}



