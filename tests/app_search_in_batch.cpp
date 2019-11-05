//
// Created by Zhen Peng on 11/4/19.
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
                 std::vector<std::vector<unsigned>> &results)
{
    std::ofstream out(filename, std::ios::binary | std::ios::out);

    for (unsigned i = 0; i < results.size(); i++) {
        unsigned GK = (unsigned) results[i].size();
        out.write((char *) &GK, sizeof(unsigned));
        out.write((char *) results[i].data(), GK * sizeof(unsigned));
    }
    out.close();
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

    unsigned L = strtoull(argv[4], nullptr, 0);
    unsigned K = strtoull(argv[5], nullptr, 0);
//    unsigned L = (unsigned) atoi(argv[4]);
//    unsigned K = (unsigned) atoi(argv[5]);

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

    int num_threads_max = 1;
    for (int num_threads = 1; num_threads < num_threads_max + 1; num_threads *= 2) {
//        omp_set_num_threads(num_threads);
//        std::vector <std::vector<unsigned>> res(query_num);
//        for (unsigned i = 0; i < query_num; i++) res[i].resize(K);
        unsigned batch_size_max = 1;
        for (unsigned batch_size = 1; batch_size <= batch_size_max; batch_size *= 2) {
            // Added by Johnpzh
            int warmup_max = 4;
            for (int warmup = 0; warmup < warmup_max; ++warmup) {
                std::vector<std::vector<unsigned>> res(query_num);
                for (unsigned i = 0; i < query_num; i++) res[i].resize(K);

                std::vector<unsigned> init_ids(L);
                boost::dynamic_bitset<> flags(index.GetSizeOfDataset());
                std::vector< std::vector<efanna2e::Neighbor> > retset_list(batch_size, std::vector<efanna2e::Neighbor>(L + 1)); // Return set
                std::vector< boost::dynamic_bitset<> > is_visited_list(batch_size, boost::dynamic_bitset<>(index.GetSizeOfDataset()));// Check flags


                unsigned remain = query_num % batch_size;
                unsigned q_i_bound = query_num - remain;
                auto s = std::chrono::high_resolution_clock::now();
                index.PrepareInitIDs(
                        init_ids,
                        flags,
                        L);
                // Prepare init_ids at first, as they are constant.

//#pragma omp parallel for
                for (unsigned q_i = 0; q_i < q_i_bound; q_i += batch_size) {
                    index.SearchWithOptGraphInBatch(
                            query_load,
                            K,
                            paras,
                            retset_list,
                            is_visited_list,
                            init_ids,
                            flags,
                            q_i,
                            batch_size,
                            res);
                }
                if (remain) {
                    index.SearchWithOptGraphInBatch(
                            query_load,
                            K,
                            paras,
                            retset_list,
                            is_visited_list,
                            init_ids,
                            flags,
                            q_i_bound,
                            remain,
                            res);
                }
//                for (unsigned i = 0; i < query_num; i++) {
//                    index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
//                }
                // Ended by Johnpzh
                auto e = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = e - s;
                // Add by Johnpzh
                {// Basic output
                    printf("batch_size: %u "
                           "L: %u "
                           "search_time(s.): %f "
                           //                       "time_distance_computation: %f "
                           "count_distance_computation: %lu "
                           "K: %u "
                           "Volume: %u "
                           "Dimension: %u "
                           "query_num: %u "
                           "query_per_sec: %f "
                           "average_latency(ms.): %f\n",
                           batch_size,
                           L,
                           diff.count(),
//                       index.time_distance_computation,
                           index.count_distance_computation,
                           K,
                           points_num,
                           dim,
                           query_num,
                           query_num / diff.count(),
                           diff.count() * 1000 / query_num);
//                index.time_distance_computation = 0.0;
                    index.count_distance_computation = 0;
                }

                // Ended by Johnpzh

                save_result(argv[6], res);
            }
        }
    }
    return 0;
}