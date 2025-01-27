//
// Created by Zhen Peng on 9/11/19.
//
/*
 * For a fixed L_SEARCH, get the mean precision for queries.
 * Required input: The true-K nearest neighbors (NN) generated by test_save_NN.
 */

#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <chrono>
#include <string>
#include <omp.h>
#include  "extras/utils.h"
//#include <math.h>

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

void read_true_NN(
        const char *filename,
        unsigned query_num,
        unsigned K,
        std::vector <std::vector< std::pair<unsigned, float> > > &true_ress)
{
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    unsigned t_query_num;
    unsigned t_K;
    fin.read(reinterpret_cast<char *>(&t_query_num), sizeof(t_query_num));
    fin.read(reinterpret_cast<char *>(&t_K), sizeof(t_K));
    if (t_query_num != query_num) {
        fprintf(stderr, "Error: query_num %u is not equal to the record %u in true-NN file %s\n",
                query_num, t_query_num, filename);
        exit(EXIT_FAILURE);
    }
    if (t_K != K) {
        fprintf(stderr, "Error: K %u is not equal to the record %u in true-NN file %s\n",
                K, t_K, filename);
        exit(EXIT_FAILURE);
    }
    true_ress.resize(t_query_num);
    for (unsigned q_i = 0; q_i < t_query_num; ++q_i) {
        true_ress[q_i].resize(t_K);
//        printf("%u:\n", q_i); //test
        for (unsigned n_i = 0; n_i < t_K; ++n_i) {
            unsigned id;
            float dist;
            fin.read(reinterpret_cast<char *>(&id), sizeof(id));
            fin.read(reinterpret_cast<char *>(&dist), sizeof(dist));
            true_ress[q_i][n_i].first = id;
            true_ress[q_i][n_i].second = dist;
//            {//test
//                printf("[%u %f]\n", id, dist);
//            }
        }
    }

    fin.close();
}

// Function: get one precision of a single query
float get_single_precision(
        const std::vector< std::pair<unsigned, float> > &query_res,
        const std::vector< std::pair<unsigned, float> > &true_res)
{
    assert(query_res.size() == true_res.size());
    size_t count = 0;
    for (const auto &qr : query_res) {
//        {//test
//            printf("query_res: [%u %f]\n", qr.first, qr.second);
//        }
        for (const auto &tr : true_res) {
//            printf("[%u %f] ", tr.first, tr.second);
            if (qr.first == tr.first) {
                ++count;
                break;
            } else if (qr.second == tr.second) {
                ++count;
                break;
            }
        }
//        printf("\n\n");
    }

    return (float) (1.0 * count / query_res.size());
}

// Function: get the mean precision
float get_final_precision(
        const std::vector<float> &precisions)
{
    double tmp_sum = 0;

    for (float p : precisions) {
        tmp_sum += p;
//        printf("p: %f\n", p);
    }
    tmp_sum /= precisions.size();
    ////////////////////////////////////
    // Geometric mean, but not suitable if zero exits
//    for (float p : precisions) {
//        tmp_sum += log(p);
//    }
//    // ret = antilog( (log(1) + log(2) + . . . + log(n)) / n )
//    tmp_sum /= precisions.size();
//    tmp_sum = exp(tmp_sum);

    return (float) tmp_sum;
}

int main(int argc, char **argv)
{
    if (argc != 8) {
        std::cout << argv[0]
                  << " data_file query_file nsg_path search_L search_K true-NN_path query_num_max"
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
//    {//test
//        printf("points_num: %u dim: %u query_num: %u\n", points_num, dim, query_num);
//        exit(1);
//    }

    unsigned L = strtoull(argv[4], nullptr, 0);
    unsigned K = strtoull(argv[5], nullptr, 0);
    {
        unsigned query_num_max = strtoull(argv[7], nullptr, 0);
        if (query_num > query_num_max) {
            query_num = query_num_max;
        }
//        printf("query_num: %u\n", query_num);
//        printf("K: %u\n", K);
    }
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

    // Read the true-NN_file
    std::vector <std::vector< std::pair<unsigned, float> > > true_ress;
    read_true_NN(argv[6], query_num, K, true_ress);
//    {// test
//        unsigned *query_true_NN = nullptr;
//        index.load_true_NN(
//                argv[6],
//                query_true_NN,
//                query_num);
//        for (unsigned q_i = 0; q_i < query_num; ++q_i) {
//            for (unsigned n_i = 0; n_i < K; ++n_i) {
//                if (true_ress[q_i][n_i].first != query_true_NN[q_i * K + n_i]) {
//                    printf("true_ress[%u][%u]: %u query_true_NN[%u]: %u\n", q_i, n_i, true_ress[q_i][n_i].first, q_i * K + n_i, query_true_NN[q_i * K + n_i]);
//                }
//            }
//        }
//        printf("Good!\n");
//        delete[] query_true_NN;
//        exit(EXIT_FAILURE);
//    }

    std::vector< std::pair<unsigned, float> > query_res(query_num); // [].first: id, [].second: distance.
    std::vector<float> precisions(query_num);
    auto s = std::chrono::high_resolution_clock::now(); // commented by Johnpzh

    // Query
    for (unsigned i = 0; i < query_num; i++) {
        index.SearchWithOptGraph(query_load + i * dim, K, paras, query_res);
        float single_precision = get_single_precision(query_res, true_ress[i]);
        precisions[i] = single_precision;
//        {//test
//            printf("single_precision: %f\n", single_precision);
//        }
    }

    // Final precision
    float final_precision = get_final_precision(precisions);
    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = e - s;
    printf("L: %u mean_precision: %f K: %u query_num: %u run_time: %f seconds\n",
            L, final_precision, K, query_num, diff.count());
//    std::cout << "run_time: " << diff.count() << "\n";

//    save_result(argv[6], res);
    delete [] data_load;
    delete [] query_load;
    return 0;
}


