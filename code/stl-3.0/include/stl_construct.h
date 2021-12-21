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

/*
 * 负责对象内容的构造与析构
 * */

#ifndef __SGI_STL_INTERNAL_CONSTRUCT_H
#define __SGI_STL_INTERNAL_CONSTRUCT_H

//#include <new.h>
#include <new> // 使用 placement new 的包含头文件
#include "stl_config.h"
#include "type_traits.h"
#include "stl_iterator.h"

__STL_BEGIN_NAMESPACE

// 第一版本 destroy 函数，接受参数为 指针
template <class T>
inline void destroy(T* pointer) {
    pointer->~T(); // 调用 T 的 析构函数
}

// placement new: 在 指针 p 所指向的内存空间创建一个 T1 类型的对象，对象的内容是从 T2 类型的对象转换而来
// 类似 realloc 将初值设定到指定的空间上
template <class T1, class T2>
inline void construct(T1* p, const T2& value) {
  new (p) T1(value);
}

// 当__type_traits为__false_type时, 调用的是下面这个函数, 通过迭代所有的对象并调用版本一的函数执行析构函数进行析构.
// 而这个是被称为non-travial destructor，就会调用自己的析构函数进行释放
template <class ForwardIterator>
inline void
__destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
  for ( ; first < last; ++first)
    destroy(&*first);
}

// 当__type_traits为__true_type时,表明由系统回收内存，所以什么都不会做
template <class ForwardIterator> 
inline void
__destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}

template <class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
  // 用于获取迭代器所指对象的类型 并定义为 trivial_destructor [用户未定义析构函数]
  typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
  // 通过类型的不一样选择执行不同的析构调用
  __destroy_aux(first, last, trivial_destructor());
}

// 第二版本的 destroy 函数 传入两个迭代器 的 destroy 函数
template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
  __destroy(first, last, value_type(first));
}

// destroy(ForwardIterator first, ForwardIterator last) 函数的特化版本
inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_CONSTRUCT_H */

// Local Variables:
// mode:C++
// End:
