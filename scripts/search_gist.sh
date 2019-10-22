#! /bin/bash
k=400
((l = k))

#output=output.gist.txt
#:> $output
#for ((l = k; l < 10*k + 1; l += k)); do
#	printf "SEARCH_L: $l SEARCH_K: $k " | tee -a $output
#	./test_nsg_optimized_search /home/zpeng/scratch/gist1m/gist_base.fvecs /home/zpeng/scratch/gist1m/gist_query.fvecs gist.nsg $l $k gist.ivecs | tee -a $output
#done

set -x
./test_nsg_optimized_search /home/zpeng/scratch/gist1m/gist_base.fvecs /home/zpeng/scratch/gist1m/gist_query.fvecs /home/zpeng/scratch/gist1m/gist.nsg $l $k gist.ivecs
set +x
