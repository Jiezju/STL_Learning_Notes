//
// Created by bright on 2022/1/5.
/* Demo Description: */

#include <iostream>

// 一般对模板参数类型typename和class认为是一样的, 可以使用 typename
template<typename T>
class people
{
public:
    typedef T    value_type;
    typedef T*    pointer;
    typedef T&    reference;
};

template<typename T>
struct man
{
public:
    //  typename告诉编译器这不是一个函数, 也不是一个变量而是一个类型
    typedef typename T::value_type    value_type;
    typedef typename T::pointer        pointer;
    typedef typename T::reference    reference;
    void print()
    {
        std::cout << "man" << std::endl;
    }
};

int main()
{
    man<people<int>> Man;
    Man.print();

    exit(0);
}
