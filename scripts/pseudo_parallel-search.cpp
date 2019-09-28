//
// Created by Zhen Peng on 9/26/19.
//

// Function: answer a bunch of queries in parallel.
// Assume queries are well-grouped into GQs. Queries in a group get the same final candidates.
void parallel_searching_1(
        Graph G,
        Start Vertex p, // searching start point
        Groups_of_queries GQs, // Well-grouped queries
        Candidate_pool_size L,
        Result_set_size K,
        fused_K_nearest_neighbors_of_queries Ss // size: GQs.size
        )
{
    Flags not_visited; // size: Qs.size
    Neighbor n;
    n.id = p;
    n.distance = 0;
    for (every query q : Qs)
        n.distance += GetDistance(n.id, q); // Get a fused distance.
    for (every group of queries Q : GQs) { // in parallel
        Index n_i = 0;
        while (n_i < L) {
            n_i = the index of the first unchecked vertex in Ss[Q];
            p_i = Ss[Q][n_i]; // the first unchecked vertex
            Mark p_i as checked;
            for (every neighbor n_id : p_i) {
                n.id = n_id;
                n.distance = 0;
                for (every query q : Q)
                    n.distance += GetDistance(n.id, q);
                Ss[Q].add(n); // Use binary-search-and-insert to make Ss[q_i] in order of the distance to Qs[q_i];
            }
            if (Ss[Q].size() > L)
                Ss[Q].resize(L);
        }
        Ss[Q].resize(K);
    }
}

// Function: answer a bunch of queries in parallel.
// Naive idea. Every query q_i maintains its own candidate queue Ss[q_i].
void parallel_searching_0(
        Graph G,
        Start Vertex p, // searching start point
        Queries Qs, // A bunch of queries
        Candidate_pool_size L,
        Result_set_size K,
        K_nearest_neighbors_of_queries Ss // size: Qs.size
        )
{
    Indices Is; // size: Qs.size
    Initial all index in Is as 0;
    Initial all sets in Ss as empty;
    for (every set S : Ss)
        S.add(p);
    for (int q_i = 0; q_i < Qs.size; ++q_i) { // In parallel
        while (Is[q_i] < L) {
            Is[q_i] = the index of the first unchecked vertex in Ss[q_i];
            p_i = Ss[q_i][Is[q_i]]; // the first unchecked vertex in Ss[q_i]
            Mark p_i as checked;
            for (every neighbor n : p_i)
                Ss[q_i].add(n); // Use binary-search-and-insert to make Ss[q_i] in order of the distance to Qs[q_i];
            if (Ss[q_i].size > L)
                Ss.resize(L);
        }
    }
    for (every set S : Ss)
        S.resize(K);
}