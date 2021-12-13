//
// Created by bright on 2021/12/14.
/* Demo Description: 函数模板使用 */

#include <iostream>
#include "../include/algorithm"

using namespace sgi_std;

class Stone
{
private:
    int m_w, m_h, m_weight;
public:
    Stone(int w, int h, int we=0)
    : m_w(w), m_h(h), m_weight(we) {}

    Stone() {}

    bool operator<(const Stone& rhs) const
    {
        return m_weight < rhs.m_weight;
    }
};

int main() {

    Stone r1(2,3), r2(3,3), r3;

    // 不需要指明 min<Stone>()
    r3 = sgi_std::min(r1,r2);

    return 0;
}

