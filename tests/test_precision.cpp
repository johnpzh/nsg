//
// Created by Zhen Peng on 9/11/19.
//

#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <chrono>
#include <string>
#include <omp.h>
#include  "extras/utils.h"

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
    if (argc != 5) {
        std::cout << argv[0]
                  << " data_file query_file nsg_path search_K"
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

    {
        printf("query_num: %u\n", query_num);
    }
    unsigned K = strtoull(argv[4], nullptr, 0);

//    if (L < K) {
//        std::cout << "search_L cannot be smaller than search_K!" << std::endl;
//        exit(-1);
//    }

    // data_load = efanna2e::data_align(data_load, points_num, dim);//one must
    // align the data before build query_load = efanna2e::data_align(query_load,
    // query_num, query_dim);
    efanna2e::IndexNSG index(dim, points_num, efanna2e::FAST_L2, nullptr);
    index.Load(argv[3]);
    index.OptimizeGraph(data_load);

    efanna2e::Parameters paras;
//    paras.Set<unsigned>("L_search", L);
//    paras.Set<unsigned>("P_search", L);

    std::vector <std::vector<unsigned>> res(query_num);
    for (unsigned i = 0; i < query_num; i++) res[i].resize(K);

    // Added by Johnpzh
    /////////////////////////////
    //
    auto s = std::chrono::high_resolution_clock::now(); // commented by Johnpzh
    for (unsigned i = 0; i < query_num; i++) {
//        index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
        index.get_true_NN(query_load + i * dim, K, res[i].data());

//        if (i == 9) exit(0); //test
    }
    //
    /////////////////////////////
    // Ended by Johnpzh
    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = e - s;
    // Add by Johnpzh
    std::cout << "search_time: " << diff.count() << "\n";
    // Ended by Johnpzh

//    save_result(argv[6], res);
    return 0;
}


