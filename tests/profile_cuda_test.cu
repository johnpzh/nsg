//
// Created by Zhen Peng on 10/22/19.
//

//#include <efanna2e/index_nsg.h>
//#include <efanna2e/util.h>
//#include <chrono>
//#include <string>
//// Added by Johnpzh
//#include <omp.h>
//#include  "extras/utils.h"
#include <stdio.h>
#include "extras/index_cuda.cuh"
//// Ended by Johnpzh


int main(int argc, char **argv)
{
    bool *d_switch;
    cudaMalloc(&d_switch, sizeof(bool));
    efanna2e::cudaPrint<<<1, 1>>>(d_switch);
    bool h_switch;
    cudaMemcpy(&h_switch, d_switch, sizeof(bool), cudaMemcpyDeviceToHost);
    cudaFree(d_switch);
    if (h_switch) {
        printf("Switched.\n");
    }

    return 0;
}
