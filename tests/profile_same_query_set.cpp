//
// Created by Zhen Peng on 10/2/19.
//
/*
 * Generate a query set of which all queries are the same.
 */

#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>

void load_data(char* filename, float*& data, unsigned& num,
               unsigned& dim)
{  // load data with sift10K pattern
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    in.read((char*)&dim, 4);
    std::cout << "data dimension: " << dim << std::endl;
    in.seekg(0, std::ios::end);
    std::ios::pos_type ss = in.tellg();
    size_t fsize = (size_t)ss;
    num = (unsigned)(fsize / (dim + 1) / 4);
    data = new float[num * dim * sizeof(float)];

    in.seekg(0, std::ios::beg);
    for (size_t i = 0; i < num; i++) {
        in.seekg(4, std::ios::cur);
        in.read((char*)(data + i * dim), dim * 4);
    }
    in.close();
}

void save_result(
        char* filename,
        float *query_load,
        unsigned query_dim,
        unsigned query_num)
{
    std::ofstream out(filename, std::ios::binary | std::ios::out);
    if (!out.is_open()) {
        fprintf(stderr, "Error: cannot create file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    for (unsigned i = 0; i < query_num; ++i) {
        out.write((char *) &query_dim, sizeof(query_dim));
        out.write((char *) query_load, query_dim * 4);
    }
    out.close();
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cout << argv[0]
                  << " query_file result_path"
                  << std::endl;
//        std::cout << argv[0]
//                  << " query_file result_path num_pairs"
//                  << std::endl;
        exit(-1);
    }
    float* query_load = NULL;
    unsigned query_num, query_dim;
    load_data(argv[1], query_load, query_num, query_dim);


    auto s = std::chrono::high_resolution_clock::now();

    // Generate the query set.
    save_result(
            argv[2],
            query_load,
            query_dim,
            query_num);
    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = e - s;
    std::cout << "running_time: " << diff.count() << "\n";

//    save_result(argv[6], res);

    return 0;
}
