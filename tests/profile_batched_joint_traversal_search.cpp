//
// Created by Zhen Peng on 10/7/19.
//
/**
 * Use Optimize Search, but queries in the query set are all the same. So they are treated as joint traversal.
 * Difference with profile_joint_traversal_search: do the joint search for one batch each time, not the whole query set.
 */
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
    efanna2e::IndexNSG index(dim, points_num, efanna2e::FAST_L2, nullptr);
    index.Load(argv[3]);
    index.OptimizeGraph(data_load);

    efanna2e::Parameters paras;
    paras.Set<unsigned>("L_search", L);
    paras.Set<unsigned>("P_search", L);

    int num_threads_max = 1;
    for (int num_threads = 1; num_threads < num_threads_max + 1; num_threads *= 2) {

        // Added by Johnpzh

        unsigned query_batch_size_max = 4096;
        for (unsigned query_batch_size = 16; query_batch_size < query_batch_size_max + 1; query_batch_size *= 2) {

            int warmup_max = 4;
            for (int warmup = 0; warmup < warmup_max; ++warmup) {
                std::vector<std::vector<unsigned>> res(query_num);
                for (unsigned i = 0; i < query_num; i++) res[i].resize(K);

                auto s = std::chrono::high_resolution_clock::now();
//#pragma omp parallel for
                unsigned remain = query_num % query_batch_size;
                unsigned q_i_bound = query_num - remain;
                for (unsigned q_i = 0; q_i < q_i_bound; q_i += query_batch_size) {
                    index.SearchWithOptGraph(
                            query_load,
                            query_num,
                            dim,
                            q_i,
                            query_batch_size,
                            K,
                            paras,
                            res);
                }
                if (remain) {
                    index.SearchWithOptGraph(
                            query_load,
                            query_num,
                            dim,
                            q_i_bound,
                            remain,
                            K,
                            paras,
                            res);
                }
//                index.SearchWithOptGraph(
//                        query_load,
//                        query_num,
//                        dim,
//                        K,
//                        paras,
//                        res);
//            for (unsigned i = 0; i < query_num; i++) {
//                index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
//            }
                // Ended by Johnpzh
                auto e = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = e - s;
                // Add by Johnpzh
                {// Basic output
                    printf("query_batch_size: %u "
                           "L: %u "
                           "search_time(s.): %f "
                           "K: %u "
                           "Volume: %u "
                           "Dimension: %u "
                           "query_num: %u "
                           "query_per_sec: %f "
                           "average_latency(ms.): %f\n",
                           query_batch_size,
                           L,
                           diff.count(),
                           K,
                           points_num,
                           dim,
                           query_num,
                           query_num / diff.count(),
                           diff.count() * 1000 / query_num);
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
    }
    return 0;
}