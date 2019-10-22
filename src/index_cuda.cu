//
// Created by Zhen Peng on 10/17/19.
//

//#include "efanna2e/index_nsg.h"
#include <stdio.h>
namespace efanna2e {

//__global__ void IndexNSG::SearchWithOptGraphOnCUDA(
//        bool *d_switch)
//{
//    int i = threadIdx.x;
//    int j = threadIdx.y;
//    int z = threadIdx.z;
//
//    *switch = true;
//
//    printf("i: %d j: %d z: %d\n", i, j, z);
//}
__global__ void cudaPrint(bool *d_switch)
{
    int i = threadIdx.x;
    int j = threadIdx.y;
    int z = threadIdx.z;

    *d_switch = true;

    printf("i: %d j: %d z: %d\n", i, j, z);
}
}