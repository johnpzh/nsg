#! /bin/bash

cd /home/zpeng/benchmarks/clion/nsg_th107b4/cmake-build-release/tests

# Generate mod for SIFT1M
data_path=/scratch/zpeng/sift1m
data_name=sift
echo "==== ${data_path}/${data_name} ===="
for ((vol = 1000000; vol >= 1000; vol /= 10)); do
	for ((dim = 128; dim >= 8; dim /= 2)); do
		./test_change_data_vol_and_dim ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${vol} ${dim} ${data_path}/mod/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/mod/${data_name}_v${vol}d${dim}_query.fvecs
	done
done
echo ""

# Generate mod for GIST1M
data_path=/scratch/zpeng/gist1m
data_name=gist
echo "==== ${data_path}/${data_name} ===="
for ((vol = 1000000; vol >= 1000; vol /= 10)); do
	for ((dim = 960; dim >= 60; dim /= 2)); do
		./test_change_data_vol_and_dim ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${vol} ${dim} ${data_path}/mod/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/mod/${data_name}_v${vol}d${dim}_query.fvecs
	done
done
echo ""

# Generate mod for DEEP10M
data_path=/scratch/zpeng/deep1b
data_name=deep10M
echo "==== ${data_path}/${data_name} ===="
for ((vol = 10000000; vol >= 1000; vol /= 10)); do
	for ((dim = 96; dim >= 6; dim /= 2)); do
		./test_change_data_vol_and_dim ${data_path}/${data_name}_base.fvecs ${data_path}/${data_name}_query.fvecs ${vol} ${dim} ${data_path}/mod/${data_name}_v${vol}d${dim}_base.fvecs ${data_path}/mod/${data_name}_v${vol}d${dim}_query.fvecs
	done
done
echo ""

