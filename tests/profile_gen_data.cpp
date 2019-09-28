//
// Created by Zhen Peng on 9/17/19.
//
/*
 * Generate synthetic data with certain volume and dimension, in random distribution.
 */
#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <chrono>
#include <string>
#include <omp.h>
#include  "extras/utils.h"

// Function: save random floats as data with certain volume and dimension.
void save_output_file(
        const char *filename,
        unsigned volume,
        unsigned dimension)
{
    std::ofstream fout(filename);
    if (!fout.is_open()) {
        fprintf(stderr, "Error: cannot create file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    srand(time(nullptr));
    std::vector<float> random_floats(dimension);
    for (unsigned i = 0; i < volume; ++i) {
        // Generate random data
        for (unsigned d_i = 0; d_i < dimension; ++d_i) {
            random_floats[d_i] = 1.0 * rand() / RAND_MAX;
        }
        fout.write(reinterpret_cast<const char *>(&dimension), 4);
        fout.write(reinterpret_cast<const char *>(random_floats.data()), dimension * 4);
    }
    fout.close();
}

int main(int argc, char **argv)
{
    if (argc != 6) {
        std::cout << argv[0]
                  << " output_data_file output_query_file volume dimension query_num"
                  << std::endl;
        exit(EXIT_FAILURE);
    }
    setbuf(stdout, nullptr); // Added by Johnpzh

    unsigned volume = strtoull(argv[3], nullptr, 0);
    unsigned dimension = strtoull(argv[4], nullptr, 0);
    unsigned query_num = strtoull(argv[5], nullptr, 0);
    auto s = std::chrono::high_resolution_clock::now();

    // Generate data_file
    save_output_file(
            argv[1],
            volume,
            dimension);

    // Generate query_file
    save_output_file(
            argv[2],
            query_num,
            dimension);

    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = e - s;
    printf("volume: %u dimension: %u query_num: %u\n",
           volume, dimension, query_num);
    std::cout << "run_time: " << diff.count() << "\n";

    return 0;
}



