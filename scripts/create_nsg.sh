#! /bin/bash

datapath=
dataname=
nndescent_bin=
nsg_index_bin=
${nndescent_bin} ${datapath}/${dataname}.fvecs ${datapath}/${dataname}_400nn.graph 400 400 12 15 100
${nsg_index_bin} ${datapath}/${dataname}.fvecs ${datapath}/${dataname}_400nn.graph 60 70 500 ${dataname}.nsg 
