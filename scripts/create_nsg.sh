#! /bin/bash

nndescent_bin=/home/zpeng/benchmarks/efanna_graph/build/tests/test_nndescent
nsg_index_bin=/home/zpeng/benchmarks/clion/nsg_th107b4/cmake-build-release/tests/test_nsg_index

## SIFT
#data_path=/scratch/zpeng/sift1m/mod
#data_name=sift
#set -x
#for ((vol = 1000000; vol >= 1000; vol /= 10)); do
#	for ((dim = 128; dim >= 8; dim /= 2)); do
#		${nndescent_bin} ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_200nn.graph 200 200 10 10 100
#		${nsg_index_bin} ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_200nn.graph 40 50 500 ${data_path}/${data_name}_v${vol}d${dim}.nsg
#	done
#done
#set +x
#
## GIST
#data_path=/scratch/zpeng/gist1m/mod
#data_name=gist
#set -x
#for ((vol = 1000000; vol >= 1000; vol /= 10)); do
#	for ((dim = 960; dim >= 60; dim /= 2)); do
#		${nndescent_bin} ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_400nn.graph 400 400 12 15 100
#		${nsg_index_bin} ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_400nn.graph 60 70 500 ${data_path}/${data_name}_v${vol}d${dim}.nsg
#	done
#done
#set +x
#
## DEEP10M
#data_path=/scratch/zpeng/deep1b/mod
#data_name=deep10M
#set -x
#for ((vol = 10000000; vol >= 1000; vol /= 10)); do
#	for ((dim = 96; dim >= 6; dim /= 2)); do
#		${nndescent_bin} ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_400nn.graph 400 400 12 15 100
#		${nsg_index_bin} ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_400nn.graph 60 70 500 ${data_path}/${data_name}_v${vol}d${dim}.nsg
#	done
#done
#set +x

# FAKE
data_path=/scratch/zpeng/fake
data_name=fake
vol_start=10000000
vol_bound=10000000
#vol_step=20000
dim_start=512
dim_bound=512
#dim_step=64
K=400

echo "==== ${data_path}/${data_name} ===="
set -x
#for ((vol = vol_start; vol <= vol_bound; vol += vol_step)); do
#	for ((dim = dim_start; dim <= dim_bound; dim += dim_step)); do
for ((vol = vol_start; vol <= vol_bound; vol *= 10)); do
	for ((dim = dim_start; dim <= dim_bound; dim *= 2)); do
		${nndescent_bin} ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_${K}nn.graph 400 400 12 15 100
		${nsg_index_bin} ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_${K}nn.graph 60 70 500 ${data_path}/${data_name}_v${vol}d${dim}.nsg
	done
done
set +x
