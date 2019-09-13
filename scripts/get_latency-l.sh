#! /bin/bash

if [[ $# -ne 4 ]]; then
	echo "Usage: ./get_L-Latency.sh <data_path> <data_name> <K> <query_num_max>"
	exit
fi

data_path=$1
data_name=$2
K=$3
query_num_max=$4

cd /home/zpeng/benchmarks/clion/nsg_th107b4/cmake-build-release/tests
for ((L = K; L < 2 * K + 1; L += K/10)); do
	./test_nsg_optimized_search ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${data_path}/${data_name}.nsg ${L} ${K} ${data_path}/output.ivecs ${query_num_max}
done

for ((L = 3 * K; L < 10 * K + 1; L += K)); do
	./test_nsg_optimized_search ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${data_path}/${data_name}.nsg ${L} ${K} ${data_path}/output.ivecs ${query_num_max}
done

L=$((50 * K))
./test_nsg_optimized_search ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${data_path}/${data_name}.nsg ${L} ${K} ${data_path}/output.ivecs ${query_num_max}

L=$((100 * K))
./test_nsg_optimized_search ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${data_path}/${data_name}.nsg ${L} ${K} ${data_path}/output.ivecs ${query_num_max}
