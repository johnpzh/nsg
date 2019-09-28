//
// Created by Zhen Peng on 9/28/2019.
//
/*
 * Shwo how man candidates are shared by two queries in every searching hop.
 */

#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <chrono>
#include <string>
// Added by Johnpzh
#include <omp.h>
#include  "extras/utils.h"
#include <unordered_set>
// Ended by Johnpzh

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

void save_result(const char *filename,
                 std::vector <std::vector<unsigned>> &results)
{
    std::ofstream out(filename, std::ios::binary | std::ios::out);

    for (unsigned i = 0; i < results.size(); i++) {
        unsigned GK = (unsigned) results[i].size();
        out.write((char *) &GK, sizeof(unsigned));
        out.write((char *) results[i].data(), GK * sizeof(unsigned));
    }
    out.close();
}

std::vector<double> get_shared_percentage_of_two_queries(
        std::vector< std::vector<unsigned> > queues_a,
        std::vector< std::vector<unsigned> > queues_b)
{
    size_t max_hops = std::max(queues_a.size(), queues_b.size());
    size_t min_hops = std::min(queues_a.size(), queues_b.size());
    std::vector<double> pertentages(max_hops);

    for (size_t q_i = 0; q_i < min_hops; ++min_hops) {
        
    }
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

    using Queues_t = std::vector< std::vector<unsigned> >;
    int num_threads_max = 1;
    for (int num_threads = 1; num_threads < num_threads_max + 1; num_threads *= 2) {
        // Added by Johnpzh
        int warmup_max = 1;
        for (int warmup = 0; warmup < warmup_max; ++warmup) {
//            std::vector<std::vector<unsigned>> res(query_num);
//            for (unsigned i = 0; i < query_num; i++) res[i].resize(K);
            std::vector<Queues_t> queues_list(query_num);

            auto s = std::chrono::high_resolution_clock::now();
//#pragma omp parallel for
            for (unsigned i = 0; i < query_num; i++) {
//                index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
                index.get_candidate_queues(query_load + i * dim, K, paras, queues_list[i]);

                // Ended by Johnpzh
                auto e = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = e - s;
                // Add by Johnpzh
                {// Basic output
                    printf("L: %u "
                           "search_time(s.): %f "
                           "K: %u "
                           "Volume: %u "
                           "Dimension: %u "
                           "query_num: %u "
                           "query_per_sec: %f "
                           "average_latency(ms.): %f\n",
                           L,
                           diff.count(),
                           K,
                           points_num,
                           dim,
                           query_num,
                           query_num / diff.count(),
                           diff.count() * 1000 / query_num);
                }
                // Ended by Johnpzh

//                save_result(argv[6], res);
            }
        }
        return 0;
    }
}