//
// Created by Zhen Peng on 9/26/19.
//

// Function: answer a bunch of queries in parallel.
void parallel_searching(
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
    for (every set S : Ss) S.add(p);
    for (int q_i = 0; q_i < Qs.size; ++q_i) {
        while (Is[q_i] < L) {
            Is[q_i] = the index of the first unchecked vertex in Ss[q_i];
            p_i = Ss[q_i][Is[q_i]]; // the first unchecked vertex in Ss[q_i]
            Mark p_i as checked;
            for (every neighbor n : p_i) {
                Ss[q_i].add(n);
            }
            Sort Ss[q_i] according to the distance to Qs[q_i];
            if (Ss[q_i].size > L) Ss.resize(L);
        }
    }
    for (every set S : Ss) Ss.resize(K);
}
