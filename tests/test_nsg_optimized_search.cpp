//
// Created by 付聪 on 2017/6/21.
//

#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <chrono>
#include <string>
// Added by Johnpzh
#include <omp.h>
#include  "extras/utils.h"
// Ended by Johnpzh

void load_data(char *filename, float *&data, unsigned &num,
               unsigned &dim) {  // load data with sift10K pattern
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cout << "open file error" << std::endl;
        exit(-1);
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

void save_result(const char *filename,
                 std::vector <std::vector<unsigned>> &results) {
    std::ofstream out(filename, std::ios::binary | std::ios::out);

    for (unsigned i = 0; i < results.size(); i++) {
        unsigned GK = (unsigned) results[i].size();
        out.write((char *) &GK, sizeof(unsigned));
        out.write((char *) results[i].data(), GK * sizeof(unsigned));
    }
    out.close();
}

int main(int argc, char **argv) {
    if (argc != 7) {
        std::cout << argv[0]
                  << " data_file query_file nsg_path search_L search_K result_path"
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
        if (query_num > 1000000) query_num = 1000000;
        printf("query_num: %u\n", query_num);
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
    efanna2e::IndexNSG index(dim, points_num, efanna2e::FAST_L2, nullptr);
    index.Load(argv[3]);
    index.OptimizeGraph(data_load);

    efanna2e::Parameters paras;
    paras.Set<unsigned>("L_search", L);
    paras.Set<unsigned>("P_search", L);

//  // Added by Johnpzh
//  query_num = 32;
//  // Ended by Johnpzh

    for (int num_threads = 1; num_threads < 37; num_threads *= 2) {

        std::vector <std::vector<unsigned>> res(query_num);
        for (unsigned i = 0; i < query_num; i++) res[i].resize(K);

        // Added by Johnpzh
        double memvirt;
        double memres;
        double memtotal;
        double memfree;
        omp_set_num_threads(num_threads);
        for (int warmup = 0; warmup < 4; ++warmup) {
            auto s = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
            for (unsigned i = 0; i < query_num; i++) {
                index.SearchWithOptGraph(query_load + i * dim, K, paras,
                                         res[i].data());
                if (0 == i) {
                    efanna2e::Utils::memory_usage(memvirt, memres);
                    efanna2e::Utils::system_memory(memtotal, memfree);
                }
            }

            /////////////////////////////
            //
//          auto s = std::chrono::high_resolution_clock::now(); // commented by Johnpzh
//          for (unsigned i = 0; i < query_num; i++) {
//            index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
//          }
            //
            /////////////////////////////
            // Ended by Johnpzh
            auto e = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = e - s;
            // Add by Johnpzh
            printf("num_threads: %u "
                   "search_time: %f "
                   "memvirt: %.2fMB memres: %.2fMB "
                   "memtotal: %.2fMB memfree: %.2fMB\n",
                   num_threads,
                   diff.count(),
                   memvirt, memres,
                   memtotal, memfree);
//        std::cout << "num_threads: " << num_threads
//                  << " search_time: " << diff.count() << "\n";
//  std::cout << "search_time: " << diff.count() << "\n";
            // Ended by Johnpzh

            save_result(argv[6], res);
        }
    }
    return 0;
}
