//
// Created by bright on 2021/12/16.
/* Demo Description: */

#include <iostream>
#include "../include/stl_construct.h"

using namespace sgi_std;

int main() {
    int a = 5;

    float b = 10.0;
    float* fPtr = &b;

    // 在 fptr 指向 的内存 构建新对象 float(5)
    construct(fPtr, a);

    std::cout << *fPtr << std::endl;

    int* iPtr = new int[10];
    destroy(iPtr, iPtr+10);

    return 0;
}

