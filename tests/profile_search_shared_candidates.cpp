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

// Function: get every percentage of shared candidates of every hop
void get_shared_percentage_of_two_queries(
        const std::vector< std::vector<unsigned> > &queues_a,
        const std::vector< std::vector<unsigned> > &queues_b,
        std::vector<double> &percentages)
{
//    size_t max_hops = std::max(queues_a.size(), queues_b.size());
    size_t min_hops = std::min(queues_a.size(), queues_b.size());

//    percentages.resize(max_hops);
    percentages.resize(min_hops);

    for (size_t q_i = 0; q_i < min_hops; ++q_i) {
        std::unordered_set<unsigned> pool;
        unsigned shared_count = 0;
        // Put queues_a[q_i] into the pool
        for (unsigned c : queues_a[q_i]) {
            pool.insert(c);
        }
        // Look up by queues_b[q_i]
        for (unsigned c : queues_b[q_i]) {
            if (pool.find(c) != pool.end()) {
                ++shared_count;
            }
        }
        // Record
        percentages[q_i] = 2.0 * shared_count / (double) (queues_a[q_i].size() + queues_b[q_i].size());
    }

//    for (size_t q_i = min_hops; q_i < max_hops; ++q_i) {
//        percentages[q_i] = 0.0;
//    }
}

int main(int argc, char **argv)
{
    if (argc != 7) {
        std::cout << argv[0]
                  << " data_file query_file nsg_path search_L search_K query_num_max"
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
    unsigned query_num_max = strtoull(argv[6], nullptr, 0);
    if (query_num < query_num_max) query_num_max = query_num;
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

    srand(time(nullptr));
    using Queues_t = std::vector< std::vector<unsigned> >;
    int num_threads_max = 1;
    for (int num_threads = 1; num_threads < num_threads_max + 1; num_threads *= 2) {
        // Added by Johnpzh
        int warmup_max = 1;
        for (int warmup = 0; warmup < warmup_max; ++warmup) {
//            std::vector<std::vector<unsigned>> res(query_num);
//            for (unsigned i = 0; i < query_num; i++) res[i].resize(K);
            std::vector<Queues_t> queues_list(query_num);
            std::vector<bool> is_selected(query_num, false);

//            auto s = std::chrono::high_resolution_clock::now();
//#pragma omp parallel for
            for (unsigned i = 0; i < query_num_max; i++) {
//                index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());

//                unsigned loc = rand() % query_num; // Random Selection
//                while (is_selected[loc]) {
//                    loc = rand() % query_num;
//                }
                unsigned loc = i; // Sequential Selection

                index.get_candidate_queues(query_load + loc * dim, K, paras, queues_list[i]);
            }
            // Ended by Johnpzh
//            auto e = std::chrono::high_resolution_clock::now();
//            std::chrono::duration<double> diff = e - s;
            // Add by Johnpzh
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
//                       query_num_max,
//                       query_num_max / diff.count(),
//                       diff.count() * 1000 / query_num_max);
//            }
            // Ended by Johnpzh
//                save_result(argv[6], res);
            { // Percentages
                for (unsigned q_i = 0; q_i < query_num_max; q_i += 2) {
                    printf("------- queries %d and %d -------\n", q_i + 1, q_i + 2);
                    std::vector<double> percentages;
                    get_shared_percentage_of_two_queries(queues_list[q_i], queues_list[q_i + 1], percentages);
                    double sum = 0;
                    for (size_t p_i = 0; p_i < percentages.size(); ++p_i) {
                        sum += percentages[p_i];
                        if (q_i == 0) {
                            printf("%lu %f\n", p_i, percentages[p_i] * 100.0);
                        }
                    }
                    printf("avg_percentage(%%): %f\n", sum / percentages.size() * 100.0);
                }
            }
        }
    }
    return 0;
}
