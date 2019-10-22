#! /bin/bash

if [[ $# -ne 4 ]]; then
	echo "Usage: ./test_L.sh <data_path> <data_name> <K> <query_num_max>"
	exit
fi

data_path=$1
data_name=$2
K=$3
query_num_max=$4

cd /home/zpeng/benchmarks/clion/nsg_th107b4/cmake-build-release/tests
for ((L = K; L < 2 * K + 1; L += 10)); do
	./test_precision ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${data_path}/${data_name}.nsg ${L} ${K} ${data_path}/${data_name}.true-${K}_NN.q-${query_num_max}.binary ${query_num_max}
done

for ((L = 3 * K; L < 10 * K + 1; L += K)); do
	./test_precision ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${data_path}/${data_name}.nsg ${L} ${K} ${data_path}/${data_name}.true-${K}_NN.q-${query_num_max}.binary ${query_num_max}
done
