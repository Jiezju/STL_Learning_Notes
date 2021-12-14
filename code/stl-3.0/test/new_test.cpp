//
// Created by bright on 2021/12/15.
/* Demo Description: new 的实现 */

#include <iostream>

const int N = 4;

using namespace std;

int main() {

    allocator<string>  alloc;
    auto str_ve = alloc.allocate(N);
    auto p = str_ve;    // vector<string> *p = str_ve;
    alloc.construct(p++);
    alloc.construct(p++, 10, 'a');
    alloc.construct(p++, "construct");
    cout << str_ve[0] << endl;
    cout << str_ve[1] << endl;
    cout << str_ve[2] << endl;

    while(p != str_ve)
    {
        alloc.destroy(--p);
    }
    alloc.deallocate(str_ve, N);

    return 0;
}

