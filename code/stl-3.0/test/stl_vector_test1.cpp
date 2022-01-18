//
// Created by bright on 2022/1/19.
/* Demo Description: */

#include <iostream>
#include "../include/vector"

using namespace sgi_std;

int main()
{
    vector<int> v1;
    vector<int> v2(4);
    vector<int> v3(4, 1);

    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);
    v1.push_back(4);

    if(!v1.empty())    // 不为空
    {
        std::cout << *v1.begin() << " " << v1.front() << " " << *(v1.end() - 1) << " " << v1.back() <<  std::endl;
        std::cout << "size = " << v1.size() << std::endl;
        v1.~vector();
    }

    exit(0);
}
