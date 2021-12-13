//
// Created by bright on 2021/12/14.
/* Demo Description: max 函数的使用 */

#include <iostream>
#include "../include/algorithm"

bool strLonger(const std::string& s1, const std::string& s2) {
    return s1.size() < s2.size();
}

int main() {

    std::cout << "max of zoo and hello: "
              << sgi_std::max(std::string("zoo"), std::string("hello"))<< std::endl;

    std::cout << "max of zoo and hello: "
              << sgi_std::max(std::string("zoo"), std::string("hello"), strLonger)<< std::endl;

    return 0;
}

