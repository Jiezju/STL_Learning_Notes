/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

/* list是用链表进行实现的, 而链表对删除, 插入的时间复杂度为O(1), 效率相当高, 但是随机访问的时间复杂度为O(n).  */

#ifndef __SGI_STL_INTERNAL_LIST_H
#define __SGI_STL_INTERNAL_LIST_H

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

// 链表节点 数据结构中就储存前后指针（双向链表）和属性
template<class T>
struct __list_node {
    typedef void* void_pointer;
    // 前后指针
    void_pointer next;
    void_pointer prev;
    // 属性
    T data;
};

template<class T, class Ref, class Ptr>
struct __list_iterator {
    typedef __list_iterator<T, T &, T *> iterator;
    typedef __list_iterator<T, const T &, const T *> const_iterator;
    typedef __list_iterator<T, Ref, Ptr> self;

    // 定义迭代器类型 bidirectional_iterator_tag ， 方便 type traits
    typedef bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef __list_node<T> *link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    link_type node;

    // 构造函数
    __list_iterator(link_type x) : node(x) {}

    __list_iterator() {}

    __list_iterator(const iterator &x) : node(x.node) {}

    bool operator==(const self &x) const { return node == x.node; }

    bool operator!=(const self &x) const { return node != x.node; }

    reference operator*() const { return (*node).data; }

#ifndef __SGI_STL_NO_ARROW_OPERATOR

    pointer operator->() const { return &(operator*()); }

#endif /* __SGI_STL_NO_ARROW_OPERATOR */

    self &operator++() {
        node = (link_type) ((*node).next);
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self &operator--() {
        node = (link_type) ((*node).prev);
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
};

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

template<class T, class Ref, class Ptr>
inline bidirectional_iterator_tag
iterator_category(const __list_iterator<T, Ref, Ptr> &) {
    return bidirectional_iterator_tag();
}

template<class T, class Ref, class Ptr>
inline T *
value_type(const __list_iterator<T, Ref, Ptr> &) {
    return 0;
}

template<class T, class Ref, class Ptr>
inline ptrdiff_t *
distance_type(const __list_iterator<T, Ref, Ptr> &) {
    return 0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

template<class T, class Alloc = alloc>
class list {
protected:
    typedef void *void_pointer;
    typedef __list_node<T> list_node;
    typedef simple_alloc <list_node, Alloc> list_node_allocator;
public:
    // 定义嵌套类型
    typedef T value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef list_node *link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

public:
    typedef __list_iterator<T, T &, T *> iterator;
    typedef __list_iterator<T, const T &, const T *> const_iterator;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
    typedef typename sgi_std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef typename sgi_std::reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
    typedef reverse_bidirectional_iterator <const_iterator, value_type,
    const_reference, difference_type>
            const_reverse_iterator;
    typedef reverse_bidirectional_iterator <iterator, value_type, reference,
    difference_type>
            reverse_iterator;
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:
    // 分配一个元素大小的空间, 返回分配的地址
    link_type get_node() { return list_node_allocator::allocate(); }

    // 释放一个元素大小的内存
    void put_node(link_type p) { list_node_allocator::deallocate(p); }

    // 分配一个元素大小的空间并调用构造初始化内存
    link_type create_node(const T &x) {
        link_type p = get_node();
        __STL_TRY{
                construct(&p->data, x);
        }
        __STL_UNWIND(put_node(p));
        return p;
    }

    // 调用析构并释放一个元素大小的空间
    void destroy_node(link_type p) {
        destroy(&p->data);
        put_node(p);
    }

protected:
    // 对节点初始化
    void empty_initialize() {
        node = get_node();
        node->next = node;
        node->prev = node;
    }

    void fill_initialize(size_type n, const T &value) {
        empty_initialize();
        __STL_TRY{
                insert(begin(), n, value);
        }
        __STL_UNWIND(clear();
        put_node(node));
    }

#ifdef __STL_MEMBER_TEMPLATES
    template <class InputIterator>
    void range_initialize(InputIterator first, InputIterator last) {
      empty_initialize();
      __STL_TRY {
        insert(begin(), first, last);
      }
      __STL_UNWIND(clear(); put_node(node));
    }
#else  /* __STL_MEMBER_TEMPLATES */

    void range_initialize(const T *first, const T *last) {
        empty_initialize();
        __STL_TRY{
                insert(begin(), first, last);
        }
        __STL_UNWIND(clear();
        put_node(node));
    }

    void range_initialize(const_iterator first, const_iterator last) {
        empty_initialize();
        __STL_TRY{
                insert(begin(), first, last);
        }
        __STL_UNWIND(clear();
        put_node(node));
    }

#endif /* __STL_MEMBER_TEMPLATES */

protected:
    /*因为node节点始终指向的一个空节点同时list是一个循环的链表, 空节点正好在头和尾的中间,
     * 所以node.next就是指向头的指针, node.prev就是指向结束的指针,
     * end返回的是最后一个数据的后一个地址也就是node
    */
    link_type node; // 定义一个节点, 这里是一个指针

public:
    // 默认构造函数, 分配一个空的node节点
    list() { empty_initialize(); }

    iterator begin() { return (link_type) ((*node).next); }

    const_iterator begin() const { return (link_type) ((*node).next); }

    iterator end() { return node; } // 返回最后一个元素的后一个的地址

    const_iterator end() const { return node; }

    // 这里是为旋转做准备, rbegin返回最后一个地址, rend返回第一个地址
    reverse_iterator rbegin() { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    // 判断是否为空链表, 这是判断只有一个空node来表示链表为空.
    bool empty() const { return node->next == node; }

    // 因为这个链表, 地址并不连续, 所以要自己迭代计算链表的长度
    size_type size() const {
        size_type result = 0;
        distance(begin(), end(), result);
        return result;
    }

    size_type max_size() const { return size_type(-1); }

    // 返回第一个元素的值
    reference front() { return *begin(); }

    const_reference front() const { return *begin(); }

    // 返回最后一个元素的值
    reference back() { return *(--end()); }

    const_reference back() const { return *(--end()); }

    void swap(list<T, Alloc> &x) { __STD::swap(node, x.node); }

    //  每一个重载函数都是直接或间接的调用该函数.
    iterator insert(iterator position, const T &x) {
        // 双向链表插入
        link_type tmp = create_node(x);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        (link_type(position.node->prev))->next = tmp;
        position.node->prev = tmp;
        return tmp;
    }

    iterator insert(iterator position) { return insert(position, T()); }

#ifdef __STL_MEMBER_TEMPLATES
    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last);
#else /* __STL_MEMBER_TEMPLATES */

    // 在指定位置上 插入 一段链表
    void insert(iterator position, const T *first, const T *last);

    void insert(iterator position,
                const_iterator first, const_iterator last);

#endif /* __STL_MEMBER_TEMPLATES */

    // 在指定位置上 插入 n 个节点
    void insert(iterator pos, size_type n, const T &x);

    void insert(iterator pos, int n, const T &x) {
        insert(pos, (size_type) n, x);
    }

    void insert(iterator pos, long n, const T &x) {
        insert(pos, (size_type) n, x);
    }

    // 头部插入节点
    void push_front(const T &x) { insert(begin(), x); }

    // 尾部插入节点
    void push_back(const T &x) { insert(end(), x); }

    /* 删除元素的操作大都是由erase函数来实现的, 其他的所有函数都是直接或间接调用erase */
    iterator erase(iterator position) {
        link_type next_node = link_type(position.node->next);
        link_type prev_node = link_type(position.node->prev);
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(position.node);
        return iterator(next_node);
    }

    iterator erase(iterator first, iterator last);

    // resize重新修改list的大小
    void resize(size_type new_size, const T &x);

    void resize(size_type new_size) { resize(new_size, T()); }

    // 删除除空节点以外的所有节点, 即只留下了最初创建的空节点.
    void clear();

    // 头部删除节点
    void pop_front() { erase(begin()); }

    // 尾部删除节点
    void pop_back() {
        iterator tmp = end();
        erase(--tmp);
    }

    // 都调用同一个函数进行初始化
    list(size_type n, const T &value) { fill_initialize(n, value); }

    list(int n, const T &value) { fill_initialize(n, value); }

    list(long n, const T &value) { fill_initialize(n, value); }

    // 分配n个节点
    explicit list(size_type n) { fill_initialize(n, T()); }

#ifdef __STL_MEMBER_TEMPLATES
    template <class InputIterator>
    list(InputIterator first, InputIterator last) {
      range_initialize(first, last);
    }

#else /* __STL_MEMBER_TEMPLATES */
    // 接受两个迭代器进行范围的初始化
    list(const T *first, const T *last) { range_initialize(first, last); }

    list(const_iterator first, const_iterator last) {
        range_initialize(first, last);
    }

#endif /* __STL_MEMBER_TEMPLATES */
    // 接受一个list参数, 进行拷贝
    list(const list<T, Alloc> &x) {
        range_initialize(x.begin(), x.end());
    }

    ~list() {
        // 删除初空节点以外的所有节点
        clear();
        // 删除空节点
        put_node(node);
    }

    // 重载赋值操作
    list<T, Alloc> &operator=(const list<T, Alloc> &x);

protected:
    // transfer函数功能是将链表中 [first, last) 内的所有元素移动到 position 之前
    // 第一个迭代器表示链表要插入的位置, first到last最闭右开区间插入到position之前.
    void transfer(iterator position, iterator first, iterator last) {
        if (position != last) {
            // last的前一个节点 的next指向插入的position节点
            (*(link_type((*last.node).prev))).next = position.node;
            // first的前一个节点 next 指向last
            (*(link_type((*first.node).prev))).next = last.node;
            // position的前一个节点的next指向first节点
            (*(link_type((*position.node).prev))).next = first.node;
            // 临时变量tmp保存position的前一个节点
            link_type tmp = link_type((*position.node).prev);
            // position的前一个节点指向last的前一个节点
            (*position.node).prev = (*last.node).prev;
            // last的前一个节点 指向first的前一个节点
            (*last.node).prev = (*first.node).prev;
            // first的prev指向tmp
            (*first.node).prev = tmp;
        }
    }

public:
    // 将两个链表进行合并. 将链表 x 插入 position 位置
    void splice(iterator position, list &x) {
        if (!x.empty())
            transfer(position, x.begin(), x.end());
    }

    // 将 i 所指向的 元素 插入 到 position 之前
    void splice(iterator position, list &, iterator i) {
        iterator j = i;
        ++j;
        if (position == i || position == j) return;
        transfer(position, i, j);
    }

    // 将 [first, last) 内的所有元素 插入 到 position 之前
    void splice(iterator position, list &, iterator first, iterator last) {
        if (first != last)
            transfer(position, first, last);
    }

    // remove 指定 value 的节点 调用erase链表清除
    void remove(const T &value);

    // unique函数是将数值相同且连续的元素删除, 只保留一个副本
    // 并不是删除所有的相同元素, 而是连续的相同元素,
    void unique();

    // 合并两个 list ， 两个 list 必须要 递增排序
    void merge(list &x);

    // 实现将链表翻转的功能
    void reverse();

    // 数据 list 与操作 sort 进行绑定，list 有自己独特的 sort 方法
    void sort();

#ifdef __STL_MEMBER_TEMPLATES
    template <class Predicate> void remove_if(Predicate);
    template <class BinaryPredicate> void unique(BinaryPredicate);
    template <class StrictWeakOrdering> void merge(list&, StrictWeakOrdering);
    template <class StrictWeakOrdering> void sort(StrictWeakOrdering);
#endif /* __STL_MEMBER_TEMPLATES */

    friend bool operator==

    __STL_NULL_TMPL_ARGS(const list &x, const list &y);
};

// 重载的 == 运算符
template<class T, class Alloc>
inline bool operator==(const list<T, Alloc> &x, const list<T, Alloc> &y) {
    typedef typename list<T, Alloc>::link_type link_type;
    link_type e1 = x.node;
    link_type e2 = y.node;
    link_type n1 = (link_type) e1->next;
    link_type n2 = (link_type) e2->next;
    for (; n1 != e1 && n2 != e2;
           n1 = (link_type) n1->next, n2 = (link_type) n2->next)
        if (n1->data != n2->data)
            return false;
    return n1 == e1 && n2 == e2;
}

// 重载的 < 运算符
template<class T, class Alloc>
inline bool operator<(const list<T, Alloc> &x, const list<T, Alloc> &y) {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class T, class Alloc>
inline void swap(list<T, Alloc>& x, list<T, Alloc>& y) {
  x.swap(y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

#ifdef __STL_MEMBER_TEMPLATES

template <class T, class Alloc> template <class InputIterator>
void list<T, Alloc>::insert(iterator position,
                            InputIterator first, InputIterator last) {
  for ( ; first != last; ++first)
    insert(position, *first);
}

#else /* __STL_MEMBER_TEMPLATES */

template<class T, class Alloc>
void list<T, Alloc>::insert(iterator position, const T *first, const T *last) {
    for (; first != last; ++first)
        insert(position, *first);
}

template<class T, class Alloc>
void list<T, Alloc>::insert(iterator position,
                            const_iterator first, const_iterator last) {
    for (; first != last; ++first)
        insert(position, *first);
}

#endif /* __STL_MEMBER_TEMPLATES */

template<class T, class Alloc>
void list<T, Alloc>::insert(iterator position, size_type n, const T &x) {
    for (; n > 0; --n)
        insert(position, x);
}

/* erase的重载, 删除两个迭代器之间的元素 */
template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first, iterator last) {
    while (first != last)
        erase(first++);
    return last;
}

template<class T, class Alloc>
void list<T, Alloc>::resize(size_type new_size, const T &x) {
    iterator i = begin();
    size_type len = 0; // 记录当前链表长度
    for (; i != end() && len < new_size; ++i, ++len);
    if (len == new_size)
        // 如果链表长度大于new_size的大小, 那就删除后面多余的节点
        erase(i, end());
    else                          // i == end()
        // i == end(), 扩大链表的节点
        insert(end(), new_size - len, x);
}

template<class T, class Alloc>
void list<T, Alloc>::clear() {
    link_type cur = (link_type) node->next;
    while (cur != node) {
        link_type tmp = cur;
        cur = (link_type) cur->next;
        destroy_node(tmp);
    }
    node->next = node;
    node->prev = node;
}

template<class T, class Alloc>
list<T, Alloc> &list<T, Alloc>::operator=(const list<T, Alloc> &x) {
    if (this != &x) {
        // first1 指向原链表  first2 指向复制的链表
        iterator first1 = begin();
        iterator last1 = end();
        const_iterator first2 = x.begin();
        const_iterator last2 = x.end();
        // 直到两个链表有一个空间用尽
        while (first1 != last1 && first2 != last2)
            *first1++ = *first2++;
        // 原链表大, 复制完后要删除掉原链表多余的元素
        if (first2 == last2)
            erase(first1, last1);
        else
            // 原链表小, 复制完后要还要将x链表的剩余元素以插入的方式插入到原链表中
            insert(last1, first2, last2);
    }
    return *this;
}

template<class T, class Alloc>
void list<T, Alloc>::remove(const T &value) {
    iterator first = begin();
    iterator last = end();
    while (first != last) {
        iterator next = first;
        ++next;
        // 删除相等的节点
        if (*first == value)
            erase(first);
        first = next;
    }
}

template<class T, class Alloc>
void list<T, Alloc>::unique() {
    iterator first = begin();
    iterator last = end();
    if (first == last) return;
    iterator next = first;
    while (++next != last) {
        if (*first == *next)
            erase(next);
        else
            first = next;
        next = first;
    }
}

template<class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc> &x) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();
    while (first1 != last1 && first2 != last2)
        if (*first2 < *first1) {
            iterator next = first2;
            // 将 [first2, first2+1) 插入到first1的前面
            transfer(first1, first2, ++next);
            first2 = next;
        } else
            ++first1;
    // 如果链表x还有元素则全部插入到first1链表的尾部
    if (first2 != last2) transfer(last1, first2, last2);
}

// 原理：将 begin() 之后的 元素 插入 到 begin() 之前，同时更新 begin 到插入的元素
template<class T, class Alloc>
void list<T, Alloc>::reverse() {
    // 空链表 或者 单个元素 的 链表 不进行 reverse
    if (node->next == node || link_type(node->next)->next == node)
        return;
    iterator first = begin();
    ++first;
    while (first != end()) {
        iterator old = first;
        ++first;
        // 将 [old, old+1) 插入 到 begin() 之前
        transfer(begin(), old, first);
    }
}

//  归并排序
/* 算法步骤 */
// sort用了一个数组链表用来存储2^i个元素, 当上一个元素存储满了之后继续往下一个链表存储, 最后将所有的链表进行merge归并(合并)
/* 1. 每次读一个数据到carry中，并将carry的数据转移到counter[0]中
       当counter[0]中的数据个数少于2时，持续转移数据到counter[0]中
       当counter[0]的数据个数等于2时，将counter[0]中的数据转移到counter[1]
       ...
       从counter[i]转移到counter[i+1],直到counter[fill]中数据个数达到2^(fill+1)个。
   2. ++fill, 重复步骤 1
 *
 * */
template<class T, class Alloc>
void list<T, Alloc>::sort() {
    // 空链表 或者 单个元素 的 链表 不进行排序
    if (node->next == node || link_type(node->next)->next == node)
        return;
    // 中转站
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
        //每次取出一个元素到 carry 中
        carry.splice(carry.begin(), *this, begin());
        int i = 0;
        while (i < fill && !counter[i].empty()) {
            counter[i].merge(carry); //将carry中的元素合并到counter[i]中
            carry.swap(counter[i++]); //交换之后counter[i-1]为空， i++
        }
        carry.swap(counter[i]);
        if (i == fill) ++fill;
    }

    // 将counter数组链表的所有节点按从小到大的顺序排列存储在counter[fill-1]的链表中
    for (int i = 1; i < fill; ++i)
        counter[i].merge(counter[i - 1]);
    swap(counter[fill - 1]);
}

#ifdef __STL_MEMBER_TEMPLATES

template <class T, class Alloc> template <class Predicate>
void list<T, Alloc>::remove_if(Predicate pred) {
  iterator first = begin();
  iterator last = end();
  while (first != last) {
    iterator next = first;
    ++next;
    if (pred(*first)) erase(first);
    first = next;
  }
}

template <class T, class Alloc> template <class BinaryPredicate>
void list<T, Alloc>::unique(BinaryPredicate binary_pred) {
  iterator first = begin();
  iterator last = end();
  if (first == last) return;
  iterator next = first;
  while (++next != last) {
    if (binary_pred(*first, *next))
      erase(next);
    else
      first = next;
    next = first;
  }
}

template <class T, class Alloc> template <class StrictWeakOrdering>
void list<T, Alloc>::merge(list<T, Alloc>& x, StrictWeakOrdering comp) {
  iterator first1 = begin();
  iterator last1 = end();
  iterator first2 = x.begin();
  iterator last2 = x.end();
  while (first1 != last1 && first2 != last2)
    if (comp(*first2, *first1)) {
      iterator next = first2;
      transfer(first1, first2, ++next);
      first2 = next;
    }
    else
      ++first1;
  if (first2 != last2) transfer(last1, first2, last2);
}

template <class T, class Alloc> template <class StrictWeakOrdering>
void list<T, Alloc>::sort(StrictWeakOrdering comp) {
  if (node->next == node || link_type(node->next)->next == node) return;
  list<T, Alloc> carry;
  list<T, Alloc> counter[64];
  int fill = 0;
  while (!empty()) {
    carry.splice(carry.begin(), *this, begin());
    int i = 0;
    while(i < fill && !counter[i].empty()) {
      counter[i].merge(carry, comp);
      carry.swap(counter[i++]);
    }
    carry.swap(counter[i]);         
    if (i == fill) ++fill;
  } 

  for (int i = 1; i < fill; ++i) counter[i].merge(counter[i-1], comp);
  swap(counter[fill-1]);
}

#endif /* __STL_MEMBER_TEMPLATES */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_LIST_H */

// Local Variables:
// mode:C++
// End:
