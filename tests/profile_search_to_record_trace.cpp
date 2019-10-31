//
// Created by Zhen Peng on 10/31/19.
//
/**
 * Do search and store the trace into file.
 * The trace contains 3 parts:
 *      1. Number of traversed vertices for current query;
 *      2. IDs of traversed vertices in order;
 *      3. Data of traversed vertices in order.
 * The file format is
 * <num_total_ids : sizeof(size_t)>
 * <num_ids_query0 : sizeof(unsigned)>
 * <id0 : sizeof(unsigned)> <norm0 : sizeof(float)> <data0 : sizeof(float) * dim>
 * <id1 : sizeof(unsigned)> <norm1 : sizeof(float)> <data1 : sizeof(float) * dim>
 * ...
 * <num_ids_query1 : sizeof(unsigned)>
 * <id0 : sizeof(unsigned)> <norm0 : sizeof(float)> <data0 : sizeof(float) * dim>
 * <id1 : sizeof(unsigned)> <norm1 : sizeof(float)> <data1 : sizeof(float) * dim>
 * ...
 * <num_ids_query2 : sizeof(unsigned)>
 * <id0 : sizeof(unsigned)> <norm0 : sizeof(float)> <data0 : sizeof(float) * dim>
 * <id1 : sizeof(unsigned)> <norm1 : sizeof(float)> <data1 : sizeof(float) * dim>
 * ...
 * ...
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

void save_result(
        const char *filename,
        const std::vector<unsigned> &trace_ids_sizes,
        const std::vector<unsigned> &trace_ids,
        const std::vector<float> &trace,
        unsigned query_num,
        unsigned dim)
{
    std::ofstream out(filename, std::ios::binary | std::ios::out);

    if (!out.is_open()) {
        fprintf(stderr, "Error: cannot create file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    size_t num_ids = trace_ids.size();
    out.write((char *)(&num_ids), sizeof(size_t));
    size_t id_start = 0;
    for (unsigned q_i = 0; q_i < query_num; ++q_i) {
        unsigned id_i_count = trace_ids_sizes[q_i];
        out.write((char *) (&id_i_count), sizeof(unsigned));
        for (unsigned id_i = 0; id_i< id_i_count; ++id_i) {
            out.write((char *) (trace_ids.data() + id_start + id_i), sizeof(unsigned));
            out.write((char *) (trace.data() + (id_start + id_i) * (dim + 1)), sizeof(float) * (dim + 1));
        }
        id_start += id_i_count;
    }
    assert(id_start == num_ids);
//    for (unsigned i = 0; i < results.size(); i++) {
//        unsigned GK = (unsigned) results[i].size();
//        out.write((char *) &GK, sizeof(unsigned));
//        out.write((char *) results[i].data(), GK * sizeof(unsigned));
//    }
//    out.close();
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

    // Record the trace
    std::vector<unsigned> trace_ids;
    std::vector<float> trace;
    std::vector<unsigned> trace_ids_sizes;
//    std::vector<unsigned> trace_sizes;
    {
        printf("Tracing...\n");
    }

    for (unsigned i = 0; i < query_num; i++) {
        unsigned start_ids = trace_ids.size();
//        unsigned start_trc = trace.size();
        index.SearchWithOptGraphToRecordTrace(query_load + i * dim, paras, trace_ids, trace);
        trace_ids_sizes.push_back(trace_ids.size() - start_ids);
//        trace_sizes.push_back(trace.size() - start_trc);
    }
    {

        printf("traces_ids.size(): %lu trace.size(): %lu\n", trace_ids.size(), trace.size());
//        printf("Computing...\n");
    }
    save_result(
            argv[6],
            trace_ids_sizes,
            trace_ids,
            trace,
            query_num,
            dim);

    return 0;
}