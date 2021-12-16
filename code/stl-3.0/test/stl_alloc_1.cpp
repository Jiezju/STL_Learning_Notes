//
// Created by bright on 2021/12/16.
/* Demo Description: */

#include <iostream>
#include "../include/stl_construct.h"

using namespace sgi_std;

class A
{
private:
    int* m_mem;
    int m_count;

public:
    A(int count=10) : m_count(count)
    {
        m_mem = new int [m_count];

        for (int i=0; i<m_count; ++i)
            m_mem[i] = i;
    }

    ~A()
    {
        delete[] m_mem;
    }

};

int main() {
    int a = 5;

    float b = 10.0;
    float* fPtr = &b;

    // 在 fptr 指向 的内存 构建新对象 float(5)
    construct(fPtr, a);

    std::cout << *fPtr << std::endl;

    int* iPtr = new int[10];
    destroy(iPtr, iPtr+10);

    // 调用 destroy 并没有真正释放
    if (iPtr == nullptr)
        std::cout << " memory has been destroyed !" << std::endl;

    A* aPtr = new A[3];
    destroy(aPtr, aPtr+3);

    return 0;
}

