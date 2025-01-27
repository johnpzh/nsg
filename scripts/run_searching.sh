#! /bin/bash

cd /home/zpeng/benchmarks/clion/nsg_th107b4/cmake-build-release/tests

## SIFT
#data_path=/scratch/zpeng/sift1m/mod
#data_name=sift
#k=200
#l=200
#echo "==== ${data_path}/${data_name} ===="
#for ((vol = 1000; vol <= 1000000; vol *= 10)); do
#	for ((dim = 8; dim <= 128; dim *= 2)); do
#		./test_nsg_optimized_search ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_query.fvecs ${data_path}/${data_name}_v${vol}d${dim}.nsg $l $k sift.ivecs 10000
#	done
#done
#
## GIST
#data_path=/scratch/zpeng/gist1m/mod
#data_name=gist
#k=400
#l=400
#echo "==== ${data_path}/${data_name} ===="
#for ((vol = 1000; vol <= 1000000; vol *= 10)); do
#	for ((dim = 60; dim <= 960; dim *= 2)); do
#		./test_nsg_optimized_search ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_query.fvecs ${data_path}/${data_name}_v${vol}d${dim}.nsg $l $k gist.ivecs 10000
#	done
#done
#
## DEEP10M
#data_path=/scratch/zpeng/deep1b/mod
#data_name=deep10M
#k=400
#l=400
#echo "==== ${data_path}/${data_name} ===="
#for ((vol = 1000; vol <= 10000000; vol *= 10)); do
#	for ((dim = 6; dim <= 96; dim *= 2)); do
#		./test_nsg_optimized_search ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_query.fvecs ${data_path}/${data_name}_v${vol}d${dim}.nsg $l $k deep10M.ivecs 10000
#	done
#done

# FAKE
data_path=/scratch/zpeng/fake
data_name=fake
vol_start=1000
vol_bound=10000000
#vol_step=20000
dim_start=64
dim_bound=512
#dim_step=64
K=400
L=400
echo "==== ${data_path}/${data_name} ===="
#for ((vol = vol_start; vol <= vol_bound; vol += vol_step)); do
#	for ((dim = dim_start; dim <= dim_bound; dim += dim_step)); do
for ((vol = vol_start; vol <= vol_bound; vol *= 10)); do
	for ((dim = dim_start; dim <= dim_bound; dim *= 2)); do
		./test_nsg_optimized_search ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_query.fvecs ${data_path}/${data_name}_v${vol}d${dim}.nsg $L $K fake.ivecs 10000
	done
done

