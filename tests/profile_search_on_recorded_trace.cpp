//
// Created by Zhen Peng on 10/25/19.
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

void load_trace_file(
        char *filename,
        std::vector<unsigned> &trace_ids_sizes,
        std::vector<unsigned> &trace_ids,
        std::vector<float> &trace,
        unsigned dim)
{
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    size_t num_ids;
    in.read((char *) (&num_ids), sizeof(size_t));
    trace_ids.resize(num_ids);
    trace.resize(num_ids * (dim + 1));
    size_t id_start = 0;
    for (unsigned q_i = 0; q_i < trace_ids_sizes.size(); ++q_i) {
        unsigned id_i_count;
        in.read((char *) (&id_i_count), sizeof(unsigned));
        trace_ids_sizes[q_i] = id_i_count;
        for (unsigned id_i = 0; id_i < id_i_count; ++id_i) {
            in.read((char *) (trace_ids.data() + id_start + id_i), sizeof(unsigned));
            in.read((char *) (trace.data() + (id_start + id_i) * (dim + 1)), sizeof(float) * (dim + 1));
        }
        id_start += id_i_count;
    }
    assert(id_start == num_ids);
}

void load_trace_file(
        char *filename,
        char *&trace,
        size_t &num_ids,
        unsigned query_num,
        unsigned dim)
{
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

//    size_t num_ids;
    in.read((char *) (&num_ids), sizeof(size_t));
    trace = (char *) malloc(num_ids * (sizeof(unsigned) + (dim + 1) * sizeof(float))
                            + query_num * sizeof(unsigned));
//    trace_ids.resize(num_ids);
//    trace.resize(num_ids * (dim + 1));
//    size_t id_start = 0;
    size_t offset = 0;
    for (unsigned q_i = 0; q_i < query_num; ++q_i) {
        unsigned id_i_count;
        in.read((char *) &id_i_count, sizeof(unsigned));
        memcpy(trace + offset, &id_i_count, sizeof(unsigned));
        offset += sizeof(unsigned);
//        trace_ids_sizes[q_i] = id_i_count;
        for (unsigned id_i = 0; id_i < id_i_count; ++id_i) {
            in.read(trace + offset, sizeof(unsigned));
            offset += sizeof(unsigned);
            in.read(trace + offset, (dim + 1) * sizeof(float));
            offset += (dim + 1) * sizeof(float);
//            in.read((char *) (trace_ids.data() + id_start + id_i), sizeof(unsigned));
//            in.read((char *) (trace.data() + (id_start + id_i) * (dim + 1)), sizeof(float) * (dim + 1));
        }
//        id_start += id_i_count;
    }
//    assert(id_start == num_ids);
}

int main(int argc, char **argv)
{
    if (argc != 9) {
        std::cout << argv[0]
                  << " data_file query_file nsg_path search_L search_K result_path query_num_max trace_file"
                  << std::endl;
        exit(-1);
    }
    setbuf(stdout, nullptr); // Added by Johnpzh
    float *data_load = NULL;
    unsigned points_num, dim;
    load_data(argv[1], data_load, points_num, dim);
    {
        free(data_load);
    }
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
//    index.Load(argv[3]);
//    index.OptimizeGraph(data_load);

    efanna2e::Parameters paras;
    paras.Set<unsigned>("L_search", L);
    paras.Set<unsigned>("P_search", L);

    // Record the trace
//    std::vector<unsigned> trace_ids;
//    std::vector<float> trace;
//    std::vector<unsigned> trace_ids_sizes(query_num);

    {
        printf("Reading...\n");
    }
    char *trace = nullptr;
    size_t num_ids;
    load_trace_file(
            argv[8],
            trace,
            num_ids,
            query_num,
            dim);
//    load_trace_file(
//            argv[8],
//            trace_ids_sizes,
//            trace_ids,
//            trace,
//            dim);
    {
        printf("num_ids: %lu\n", num_ids);
        printf("Computing...\n");
    }
    int num_threads_max = 1;
    for (int num_threads = 1; num_threads < num_threads_max + 1; num_threads *= 2) {
//        omp_set_num_threads(num_threads);

        // Added by Johnpzh
        int warmup_max = 4;
        for (int warmup = 0; warmup < warmup_max; ++warmup) {
            std::vector<std::vector<unsigned>> res(query_num);
            for (unsigned i = 0; i < query_num; i++) res[i].resize(K);
//            unsigned *trace_ids_ptr = trace_ids.data();
//            float *trace_ptr = trace.data();
            auto s = std::chrono::high_resolution_clock::now();
//#pragma omp parallel for
            size_t offset = 0;
            for (unsigned i = 0; i < query_num; i++) {
//                index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());

                unsigned ids_count = *((unsigned *) (trace + offset));
                offset += sizeof(unsigned);
                index.SearchWithOptGraphAndTrace(
                        query_load + i * dim,
                        K,
                        trace + offset,
                        ids_count,
                        paras,
                        res[i].data());
                offset += ids_count * (sizeof(unsigned) + (dim + 1) * sizeof(float));
//                unsigned ids_count = trace_ids_sizes[i];
//                index.SearchWithOptGraphAndTrace(
//                        query_load + i * dim,
//                        K,
//                        trace_ids_ptr,
//                        trace_ptr,
//                        ids_count,
//                        paras,
//                        res[i].data());
//                trace_ids_ptr += ids_count;
//                trace_ptr += ids_count * (dim + 1);
//                trace_ptr += trace_sizes[i];
            }
            // Ended by Johnpzh
            auto e = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = e - s;
            // Add by Johnpzh
            {// Basic output
                printf("L: %u "
                       "search_time(s.): %f "
//                       "time_distance_computation: %f "
//                       "count_neighbors_hops: %lu "
                       "K: %u "
                       "Volume: %u "
                       "Dimension: %u "
                       "query_num: %u "
                       "query_per_sec: %f "
                       "average_latency(ms.): %f\n",
                       L,
                       diff.count(),
//                       index.time_distance_computation,
//                       index.count_distance_computation,
                       K,
                       points_num,
                       dim,
                       query_num,
                       query_num / diff.count(),
                       diff.count() * 1000 / query_num);
//                index.time_distance_computation = 0.0;
//                index.count_distance_computation = 0;
            }
            // Ended by Johnpzh

            save_result(argv[6], res);
        }
    }
    free(query_load);
    free(trace);
    return 0;
}