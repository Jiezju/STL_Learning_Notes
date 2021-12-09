//
// Created by bright on 2021/12/9.
/* Demo Description: iter visit && for range-base */

#include <iostream>
#include "../include/vector"
#include "../include/list"
#include "../include/algorithm"
#include <string>

using namespace sgi_std;

int main()
{
    vector<int> iv(5, 1);

    /* iterator */
    vector<int>::iterator ite = iv.begin();

    for (; ite != iv.end(); ++ite) {
        std::cout << *ite << std::endl;
    }

    /* for range base */
    for (auto ele : iv) {
        std::cout << ele << std::endl;
    }

    // reference
    for (auto& ele : iv) {
        ele *= 2;
    }

    for (auto ele : iv) {
        std::cout << ele << std::endl;
    }

    /* auto */
    list<std::string, alloc> messages;//声明了list＜string＞模板类的一个实例
    messages.push_back("Chocolate"); //实例化
    messages.push_back("Strawberry");
    messages.push_front("Lime");
    messages.push_front("Vanilla");

    auto s_ite = find(messages.begin(), messages.end(), "Vanilla");

    std::cout << *s_ite << std::endl;

    return 0;
}
