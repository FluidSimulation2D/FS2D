#include "add.cuh"

__global__ void add(int a, int b, int *c)
{
    *c = a + b;
}

int add(int a, int b)
{
    int result, *localResult;

    cudaMalloc((void**)&localResult, sizeof(int));
    add<<<1,1>>>(a, b, localResult);
    cudaMemcpy(&result, localResult, sizeof(int), cudaMemcpyDeviceToHost);
    cudaFree(localResult);

    return result;
}
