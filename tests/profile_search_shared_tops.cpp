//
// Created by Zhen Peng on 10/11/19.
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

double get_shared_percentage_of_tops(
        const std::vector<unsigned> &tops_a,
        const std::vector<unsigned> &tops_b)
{
    size_t min_hops = std::min(tops_a.size(), tops_b.size());
    size_t shared_count = 0;
    for (size_t h_i = 0; h_i < min_hops; ++h_i) {
        if (tops_a[h_i] == tops_b[h_i]) {
            ++shared_count;
        }
    }

    return 1.0 * shared_count / min_hops;
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

    int num_threads_max = 1;
    for (int num_threads = 1; num_threads < num_threads_max + 1; num_threads *= 2) {

        // Added by Johnpzh
        int warmup_max = 1;
        for (int warmup = 0; warmup < warmup_max; ++warmup) {
//            std::vector<std::vector<unsigned>> res(query_num);
//            for (unsigned i = 0; i < query_num; i++) res[i].resize(K);
            std::vector< std::vector<unsigned> > tops_list(query_num);

            auto s = std::chrono::high_resolution_clock::now();
//#pragma omp parallel for
            for (unsigned i = 0; i < query_num; i++) {
                index.get_top_ranks(
                        query_load + i * dim,
                        K,
                        paras,
                        tops_list[i]);
            }
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
            {// Shared Percentage
                double avg_ratio = 0;
                double max_ratio = 0;
                double min_ratio = 1;
                for (unsigned q_i = 0; q_i < query_num_max; q_i += 2) {
                    double ratio = get_shared_percentage_of_tops(tops_list[q_i], tops_list[q_i + 1]);
//                    if (ratio != 0.0) {
//                        printf("%u_vs_%u: %f\n", q_i, q_i + 1, ratio);
//                    }
                    if (ratio > max_ratio) {
                        max_ratio = ratio;
                    } else if (ratio < min_ratio) {
                        min_ratio = ratio;
                    }
                    avg_ratio += ratio;
                }
                avg_ratio /= (query_num_max / 2);
                printf("num_pairs: %u avg_ratio: %f min_ratio: %f max_ratio: %f\n", query_num_max / 2, avg_ratio, min_ratio, max_ratio);
            }
            // Ended by Johnpzh

//            save_result(argv[6], res);
        }
    }
    return 0;
}
