//
// Created by Zhen Peng on 10/1/19.
//
/*
 * For a query, find its nearest query in the query set to form a pair of queries.
 * Find out all pairs of queries and save them sequentially into a new query file.
 */
#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>

void load_data(char* filename, float*& data, unsigned& num,
               unsigned& dim)
{  // load data with sift10K pattern
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    in.read((char*)&dim, 4);
    std::cout << "data dimension: " << dim << std::endl;
    in.seekg(0, std::ios::end);
    std::ios::pos_type ss = in.tellg();
    size_t fsize = (size_t)ss;
    num = (unsigned)(fsize / (dim + 1) / 4);
    data = new float[num * dim * sizeof(float)];

    in.seekg(0, std::ios::beg);
    for (size_t i = 0; i < num; i++) {
        in.seekg(4, std::ios::cur);
        in.read((char*)(data + i * dim), dim * 4);
    }
    in.close();
}

void save_result(
        char* filename,
        float *query_load,
        unsigned query_dim,
        std::vector< std::pair<unsigned, unsigned> >& id_pairs)
{
    std::ofstream out(filename, std::ios::binary | std::ios::out);
    if (!out.is_open()) {
        fprintf(stderr, "Error: cannot create file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    for (const auto & id_p : id_pairs) {
        out.write((char *) &query_dim, sizeof(query_dim));
        out.write((char *) (query_load + query_dim * id_p.first), query_dim * 4);
        out.write((char *) &query_dim, sizeof(query_dim));
        out.write((char *) (query_load + query_dim * id_p.second), query_dim * 4);
    }
    out.close();
}

void do_pair(
        float *query_load,
        unsigned query_num,
        unsigned query_dim,
        unsigned num_pairs,
        std::vector< std::pair<unsigned, unsigned> > &id_pairs)
{
    efanna2e::DistanceL2 dst_obj;
    std::vector<bool> is_selected(query_num, false);
    unsigned pair_id = 0;
    for (unsigned q_i = 0; q_i < query_num; ++q_i) {
        float dst_min = (uint64_t) -1;
        unsigned id_min;
        float* q = query_load + q_i * query_dim;
        if (is_selected[q_i]) {
            continue;
        }
        is_selected[q_i] = true;
        for (unsigned c_i = 0; c_i < query_num; ++c_i) {
            if (is_selected[c_i]) {
                continue;
            }
            float dst = dst_obj.compare(q,
                                query_load + c_i * query_dim,
                                query_dim);
            if (dst < dst_min) {
                dst_min = dst;
                id_min = c_i;
            }
        }
        id_pairs[pair_id++] = std::make_pair(q_i, id_min);
        if (pair_id == num_pairs) {
            break;
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        std::cout << argv[0]
                  << " query_file result_path num_pairs"
                  << std::endl;
//        std::cout << argv[0]
//                  << " data_file query_file nsg_path search_L search_K result_path"
//                  << std::endl;
        exit(-1);
    }
//    float* data_load = NULL;
//    unsigned points_num, dim;
//    load_data(argv[1], data_load, points_num, dim);
    float* query_load = NULL;
    unsigned query_num, query_dim;
    load_data(argv[1], query_load, query_num, query_dim);
//    assert(dim == query_dim);

//    unsigned L = (unsigned)atoi(argv[4]);
//    unsigned K = (unsigned)atoi(argv[5]);
    unsigned num_pairs = strtoull(argv[3], nullptr, 0);
    if (num_pairs * 2 > query_num) {
        fprintf(stderr, "Error: total %u queries cannot get %u pairs of queries.\n", query_num, num_pairs);
        exit(EXIT_FAILURE);
    }
    std::vector< std::pair<unsigned, unsigned> > id_pairs(num_pairs);

    auto s = std::chrono::high_resolution_clock::now();

    // Find pairs of queries.
    do_pair(
            query_load,
            query_num,
            query_dim,
            num_pairs,
            id_pairs);

    // Store into the file.
    save_result(
            argv[2],
            query_load,
            query_dim,
            id_pairs);
    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = e - s;
    std::cout << "running_time: " << diff.count() << "\n";

//    save_result(argv[6], res);

    return 0;
}
