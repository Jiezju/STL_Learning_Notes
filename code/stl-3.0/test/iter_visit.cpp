//
// Created by bright on 2021/12/9.
/* Demo Description: iter visit */

#include <iostream>
#include "../include/vector"

using namespace sgi_std;

int main()
{
    vector<int> iv(5, 1);
    auto ite = iv.begin();

    for (; ite != iv.end(); ++ite) {
        std::cout << *ite << std::endl;
    }

    return 0;
}
