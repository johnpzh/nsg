#! /bin/bash

cd /home/zpeng/benchmarks/clion/nsg_th107b4/cmake-build-release/tests

# Create fake data with volumes and dimensions
data_path=/scratch/zpeng/fake
data_name=fake
vol_start=20000
vol_bound=200000
vol_step=20000
dim_start=64
dim_bound=512
dim_step=64
query_num=10000

echo "==== ${data_path}/${data_name} ===="
set -x
for ((vol = vol_start; vol <= vol_bound; vol += vol_step)); do
	for ((dim = dim_start; dim <= dim_bound; dim += dim_step)); do
		./test_gen_data ${data_path}/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/${data_name}_v${vol}d${dim}_query.fvecs ${vol} ${dim} ${query_num}
	done
done
set +x
