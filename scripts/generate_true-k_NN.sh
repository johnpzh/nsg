#! /bin/bash

query_num_max=1000

cd /home/zpeng/benchmarks/clion/nsg_th107b4/cmake-build-release/tests

## Generate true-200_NN for SIFT1M
#K=200
#data_path=/scratch/zpeng/sift1m
#data_name=sift
#echo "==== ${data_path}/${data_name} ===="
#./test_save_NN ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${data_path}/${data_name}.nsg ${K} ${data_path}/${data_name}.true-${K}_NN.q-${query_num_max}.binary ${query_num_max}
#echo ""
#
## Generate true-400_NN for GIST1M
#K=400
#data_path=/scratch/zpeng/gist1m
#data_name=gist
#echo "==== ${data_path}/${data_name} ===="
#./test_save_NN ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${data_path}/${data_name}.nsg ${K} ${data_path}/${data_name}.true-${K}_NN.q-${query_num_max}.binary ${query_num_max}
#echo ""

# Generate true-400_NN for DEEP10M
K=400
data_path=/scratch/zpeng/deep1b
data_name=deep10M
echo "==== ${data_path}/${data_name} ===="
./test_save_NN ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${data_path}/${data_name}.nsg ${K} ${data_path}/${data_name}.true-${K}_NN.q-${query_num_max}.binary ${query_num_max}
