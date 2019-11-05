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

        // Added by Johnpzh
        int warmup_max = 4;
        for (int warmup = 0; warmup < warmup_max; ++warmup) {
            std::vector<std::vector<unsigned>> res(query_num);
            for (unsigned i = 0; i < query_num; i++) res[i].resize(K);

            auto s = std::chrono::high_resolution_clock::now();
//#pragma omp parallel for
            for (unsigned i = 0; i < query_num; i++) {
                index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
            }
            ///////////////////////////////
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
            {// Basic output
                printf("L: %u "
                       "search_time(s.): %f "
//                       "time_distance_computation: %f "
                       "count_distance_computation: %lu "
                       "K: %u "
                       "Volume: %u "
                       "Dimension: %u "
                       "query_num: %u "
                       "query_per_sec: %f "
                       "average_latency(ms.): %f\n",
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

//            printf("num_threads: %u "
//                   "search_time: %f "
//                   "memvirt: %.2fMB memres: %.2fMB "
//                   "memtotal: %.2fMB memfree: %.2fMB\n",
//                   num_threads,
//                   diff.count(),
//                   memvirt, memres,
//                   memtotal, memfree);
            // Ended by Johnpzh

            save_result(argv[6], res);
        }
    }
    return 0;
}


//// Backup: some measurement
//// distance computation; average hops; average latency for top-1st and top-Kth
//int main(int argc, char **argv) {
//    if (argc != 8) {
//        std::cout << argv[0]
//                  << " data_file query_file nsg_path search_L search_K result_path query_num_max"
//                  << std::endl;
//        exit(-1);
//    }
//    setbuf(stdout, nullptr); // Added by Johnpzh
//    float *data_load = NULL;
//    unsigned points_num, dim;
//    load_data(argv[1], data_load, points_num, dim);
//    float *query_load = NULL;
//    unsigned query_num, query_dim;
//    load_data(argv[2], query_load, query_num, query_dim);
//    assert(dim == query_dim);
//    // Added by Johnpzh
//    {
//        unsigned query_num_max = strtoull(argv[7], nullptr, 0);
//        if (query_num > query_num_max) query_num = query_num_max;
////        printf("query_num: %u\n", query_num);
//    }
//    // Ended by Johnpzh
//
//    unsigned L = (unsigned) atoi(argv[4]);
//    unsigned K = (unsigned) atoi(argv[5]);
//
//    if (L < K) {
//        std::cout << "search_L cannot be smaller than search_K!" << std::endl;
//        exit(-1);
//    }
//
//    // data_load = efanna2e::data_align(data_load, points_num, dim);//one must
//    // align the data before build query_load = efanna2e::data_align(query_load,
//    // query_num, query_dim);
//    efanna2e::IndexNSG index(dim, points_num, efanna2e::FAST_L2, nullptr);
//    index.Load(argv[3]);
//    index.OptimizeGraph(data_load);
//
//    efanna2e::Parameters paras;
//    paras.Set<unsigned>("L_search", L);
//    paras.Set<unsigned>("P_search", L);
//
////  // Added by Johnpzh
////  query_num = 32;
////  // Ended by Johnpzh
//
//    int num_threads_max = 1;
//    for (int num_threads = 1; num_threads < num_threads_max + 1; num_threads *= 2) {
////
////        std::vector <std::vector<unsigned>> res(query_num);
////        for (unsigned i = 0; i < query_num; i++) res[i].resize(K);
//
//        // Added by Johnpzh
////        double memvirt;
////        double memres;
////        double memtotal;
////        double memfree;
////        omp_set_num_threads(num_threads);
////        {// Added by Johnpzh: measure a query's top-L neighbors' latencies
////            index.time_neighbors_latencies.resize(L);
////            index.count_neighbors_hops.resize(L);
////        }
//        int warmup_max = 4;
//        for (int warmup = 0; warmup < warmup_max; ++warmup) {
////            // For measurement only
////            uint64_t distance_computation_count = 0;
////            std::vector<double> top_one_latencies(query_num); // used with time_neighbors_latencies
////            std::vector<double> top_k_latencies(query_num); // used with time_neighbors_latencies
////            std::vector<uint32_t > top_one_hops(query_num); // used with count_neighbors_hops
////            std::vector<uint32_t> top_k_hops(query_num); // used with count_neighbors_hops
//
//            std::vector <std::vector<unsigned>> res(query_num);
//            for (unsigned i = 0; i < query_num; i++) res[i].resize(K);
//
//            auto s = std::chrono::high_resolution_clock::now();
////#pragma omp parallel for
//            for (unsigned i = 0; i < query_num; i++) {
////                {// Added by Johnpzh: measure a query's top-L neighbors' latencies
////                    for (unsigned n_i = 0; n_i < L; ++n_i) {
////                        index.time_neighbors_latencies[n_i].first = efanna2e::Utils::get_time_mark(); // start time
////                    }
////
////                    // Hops
////                    for (unsigned n_i = 0; n_i < L; ++n_i) {
////                        index.count_neighbors_hops[n_i] = 0;
////                    }
////
////                    // Distance computation count
////                    index.count_distance_computation = 0;
////
////                }
//                index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
////                if (0 == i) {
////                    efanna2e::Utils::memory_usage(memvirt, memres);
////                    efanna2e::Utils::system_memory(memtotal, memfree);
////                }
////                {// Added by Johnpzh: measure a query's top-L neighbors' latencies
////                    top_one_latencies[i] = index.time_neighbors_latencies[0].second;
////                    top_k_latencies[i] = index.time_neighbors_latencies[K - 1].second;
////
////                    // Hops
////                    top_one_hops[i] = index.count_neighbors_hops[0];
////                    top_k_hops[i] = index.count_neighbors_hops[K - 1];
////
////                    // Distance computation count
//////                    printf("distance_computation_count: %lu\n", index.count_distance_computation);
////                    distance_computation_count += index.count_distance_computation;
////                }
//            }
//
//            ///////////////////////////////
//            //
////          auto s = std::chrono::high_resolution_clock::now(); // commented by Johnpzh
////          for (unsigned i = 0; i < query_num; i++) {
////            index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
////          }
//            //
//            /////////////////////////////
//            // Ended by Johnpzh
//            auto e = std::chrono::high_resolution_clock::now();
//            std::chrono::duration<double> diff = e - s;
//            // Add by Johnpzh
////            {
////                // For latencies of top-K neighbors
////                double latency_one = 0;
////                for (unsigned q_i = 0; q_i < query_num; ++q_i) {
////                    latency_one += top_one_latencies[q_i];
////                }
////                latency_one /= query_num;
////
////                double latency_k = 0;
////                for (unsigned q_i = 0; q_i < query_num; ++q_i) {
////                    latency_k += top_k_latencies[q_i];
////                }
////                latency_k /= query_num;
////
////                // For hops
////                uint64_t hops_one = 0;
////                uint64_t hops_k = 0;
////                for (unsigned q_i = 0; q_i < query_num; ++q_i) {
////                    hops_one += top_one_hops[q_i];
////                    hops_k += top_k_hops[q_i];
////                }
////
////                printf("L: %u "
////                       "search_time(s.): %f "
////                       "K: %u "
////                       "Volume: %u "
////                       "Dimension: %u "
////                       "query_num: %u "
////                       "query_per_sec: %f "
////                       "average_latency(ms.): %f "
////                       "Top-1st_avg_latency(ms.): %f "
////                       "Top-%uth_avg_latency(ms.): %f "
////                       "distance_computation_avg_count: %lu "
////                       "Top-1st_avg_hops: %f "
////                       "Top-%uth_avg_hops: %f\n",
////                       L,
////                       diff.count(),
////                       K,
////                       points_num,
////                       dim,
////                       query_num,
////                       query_num / diff.count(),
////                       diff.count() * 1000 / query_num,
////                       latency_one * 1000,
////                       K, latency_k * 1000,
////                       distance_computation_count / query_num,
////                       1.0 * hops_one / query_num,
////                       K, 1.0 * hops_k / query_num);
////            }
//
//            {// Basic output
//                printf("L: %u "
//                       "search_time(s.): %f "
//                       "K: %u "
//                       "Volume: %u "
//                       "Dimension: %u "
//                       "query_num: %u "
//                       "query_per_sec: %f "
//                       "average_latency(ms.): %f\n",
//                       L,
//                       diff.count(),
//                       K,
//                       points_num,
//                       dim,
//                       query_num,
//                       query_num / diff.count(),
//                       diff.count() * 1000 / query_num);
//            }
//
////            printf("num_threads: %u "
////                   "search_time: %f "
////                   "memvirt: %.2fMB memres: %.2fMB "
////                   "memtotal: %.2fMB memfree: %.2fMB\n",
////                   num_threads,
////                   diff.count(),
////                   memvirt, memres,
////                   memtotal, memfree);
//            // Ended by Johnpzh
//
//            save_result(argv[6], res);
//        }
//    }
//    return 0;
//}
