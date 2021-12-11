//
// Created by bright on 2021/12/8.
/* Demo Description:  Six Components */

#include "../include/vector"
#include "../include/algorithm"
#include "../include/functional"
#include <iostream>

using namespace sgi_std;

int main()
{
    int ia[6] = {27, 210, 12, 47, 109, 83};
    // SGI STL 的配置器，其名称是 alloc 而不是 allocator，而且不接受任何参数
    vector<int, alloc> vi(ia, ia+6);

    auto res = count_if(vi.begin(), vi.end(),not1(bind2nd(less<int>(), 40)));

    std::cout << res << std::endl;

    return 0;
}


