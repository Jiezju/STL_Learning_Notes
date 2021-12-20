//
// Created by bright on 2021/12/17.
/* Demo Description: 第一级内存配置器 */

#include <iostream>
#include "../include/vector"
#include "../include/stl_alloc.h"

using namespace sgi_std;

int main() {

    int ia[6] = {27, 210, 12, 47, 109, 83};
    // SGI STL 的配置器，其名称是 alloc 而不是 allocator，而且不接受任何参数
    vector<int, malloc_alloc> vi(ia, ia+6);

    // 使用 第二级分配器进行内存分配
    vector<int, alloc> va(ia, ia+6);

    return 0;
}

