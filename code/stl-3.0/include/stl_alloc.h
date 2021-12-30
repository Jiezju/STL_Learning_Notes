/*
 * Copyright (c) 1996-1997
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
 * 负责内存空间的配置和释放
 * */

#ifndef __SGI_STL_INTERNAL_ALLOC_H
#define __SGI_STL_INTERNAL_ALLOC_H

#ifdef __SUNPRO_CC
#  define __PRIVATE public
   // Extra access restrictions prevent us from really making some things
   // private.
#else
#  define __PRIVATE private
#endif

#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
#  define __USE_MALLOC
#endif


// This implements some standard node allocators.  These are
// NOT the same as the allocators in the C++ draft standard or in
// in the original STL.  They do not encapsulate different pointer
// types; indeed we assume that there is only one pointer type.
// The allocation primitives are intended to allocate individual objects,
// not larger arenas as with the original STL allocators.

#if 0
#   include <new>
#   define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
//#   include <iostream.h>
#	include <iostream>
#   define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1)
#endif

#ifndef __ALLOC
#   define __ALLOC alloc
#endif
#ifdef __STL_WIN32THREADS
#   include <windows.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef __RESTRICT
#  define __RESTRICT
#endif

#if !defined(_PTHREADS) && !defined(_NOTHREADS) \
 && !defined(__STL_SGI_THREADS) && !defined(__STL_WIN32THREADS)
#   define _NOTHREADS
#endif

# ifdef _PTHREADS
    // POSIX Threads
    // This is dubious, since this is likely to be a high contention
    // lock.   Performance may not be adequate.
#   include <pthread.h>
#   define __NODE_ALLOCATOR_LOCK \
        if (threads) pthread_mutex_lock(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_UNLOCK \
        if (threads) pthread_mutex_unlock(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_THREADS true
#   define __VOLATILE volatile  // Needed at -O3 on SGI
# endif
# ifdef __STL_WIN32THREADS
    // The lock needs to be initialized by constructing an allocator
    // objects of the right type.  We do that here explicitly for alloc.
#   define __NODE_ALLOCATOR_LOCK \
        EnterCriticalSection(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_UNLOCK \
        LeaveCriticalSection(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_THREADS true
#   define __VOLATILE volatile  // may not be needed
# endif /* WIN32THREADS */
# ifdef __STL_SGI_THREADS
    // This should work without threads, with sproc threads, or with
    // pthreads.  It is suboptimal in all cases.
    // It is unlikely to even compile on nonSGI machines.

    extern "C" {
      extern int __us_rsthread_malloc;
    }
	// The above is copied from malloc.h.  Including <malloc.h>
	// would be cleaner but fails with certain levels of standard
	// conformance.
#   define __NODE_ALLOCATOR_LOCK if (threads && __us_rsthread_malloc) \
                { __lock(&__node_allocator_lock); }
#   define __NODE_ALLOCATOR_UNLOCK if (threads && __us_rsthread_malloc) \
                { __unlock(&__node_allocator_lock); }
#   define __NODE_ALLOCATOR_THREADS true
#   define __VOLATILE volatile  // Needed at -O3 on SGI
# endif
# ifdef _NOTHREADS
//  Thread-unsafe
#   define __NODE_ALLOCATOR_LOCK
#   define __NODE_ALLOCATOR_UNLOCK
#   define __NODE_ALLOCATOR_THREADS false
#   define __VOLATILE
# endif

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

// Malloc-based allocator.  Typically slower than default alloc below.
// Typically thread-safe and more storage efficient.
#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
# ifdef __DECLARE_GLOBALS_HERE
    void (* __malloc_alloc_oom_handler)() = 0;
    // g++ 2.7.2 does not handle static template data members.
# else
    extern void (* __malloc_alloc_oom_handler)();
# endif
#endif

// 一级配置器
template <int inst>
class __malloc_alloc_template {

// 这里private里面的函数都是在内存不足的时候进行调用的
private:

// 三个私有函数 处理 内存不足的 情况
static void *oom_malloc(size_t); // oom: out of memory

static void *oom_realloc(void *, size_t);

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
    // 内存不足设置的处理例程, 默认设置的是0, 表示没有设置处理例程, 这个处理例程是由用户手动设置的
    static void (* __malloc_alloc_oom_handler)();
#endif

public:

static void * allocate(size_t n)
{
    // 这里直接调用malloc分配内存
    void *result = malloc(n);
    // 分配失败 继续调用oom_malloc来选择抛出异常还是一直申请内存, 直到申请内存成功.
    if (0 == result) result = oom_malloc(n);
    return result;
}

static void deallocate(void *p, size_t /* n */)
{
    // 直接调用 free
    free(p);
}

static void * reallocate(void *p, size_t /* old_sz */, size_t new_sz)
{
    void * result = realloc(p, new_sz);
    if (0 == result) result = oom_realloc(p, new_sz);
    return result;
}

/************************************************/
/*
 * 等价代码
 * typedef void (*H)(); // 定义函数指针
 * static H set_malloc_handler(H f)
 *
 * */
static void (* set_malloc_handler(void (*f)()))()
{
    void (* old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = f;
    return(old);
}

};

// malloc_alloc out-of-memory handling

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
// 由 用户指定的 内存不足 处理函数
template <int inst>
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;
#endif

template <int inst>
void * __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
    void (* my_malloc_handler)();
    void *result;

    // 用户自定义处理例程, 就一直申请内存, 否则抛出异常
    for (;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*my_malloc_handler)();
        result = malloc(n);
        if (result) return(result);
    }
}

template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
    void (* my_malloc_handler)();
    void *result;

    for (;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*my_malloc_handler)();
        result = realloc(p, n);
        if (result) return(result);
    }
}

// 程序默认定义mallo_alloc函数, 并且设置统一的调用接口, 默认的的接口为第二级配置器
// 默认将 inst 设为0;
typedef __malloc_alloc_template<0> malloc_alloc;

/*
 * 定义符合STL规格的配置器接口, 不管是一级配置器还是二级配置器都是使用这个接口进行分配的
 * 容器指定的 alloc 分配器 通过 调用 simple_alloc<T, alloc> 实现分配
 * 通过 Alloc::allocate 分配内存
 * Alloc::deallocate 释放内存
 *
 * container<T, Alloc> -> container() -> simple_alloc<T, Alloc> -> Alloc::allocate
 * ~container<T, Alloc> -> ~container() -> simple_alloc<T, Alloc> -> Alloc::deallocate
 *
 * */
template<class T, class Alloc>
class simple_alloc {

public:
    static T *allocate(size_t n)
                { return 0 == n? 0 : (T*) Alloc::allocate(n * sizeof (T)); }
    static T *allocate(void)
                { return (T*) Alloc::allocate(sizeof (T)); }
    static void deallocate(T *p, size_t n)
                { if (0 != n) Alloc::deallocate(p, n * sizeof (T)); }
    static void deallocate(T *p)
                { Alloc::deallocate(p, sizeof (T)); }
};

// Allocator adaptor to check size arguments for debugging.
// Reports errors using assert.  Checking can be disabled with
// NDEBUG, but it's far better to just use the underlying allocator
// instead when no checking is desired.
// There is some evidence that this can confuse Purify.
template <class Alloc>
class debug_alloc {

private:

enum {extra = 8};       // Size of space used to store size.  Note
                        // that this must be large enough to preserve
                        // alignment.

public:

static void * allocate(size_t n)
{
    char *result = (char *)Alloc::allocate(n + extra);
    *(size_t *)result = n;
    return result + extra;
}

static void deallocate(void *p, size_t n)
{
    char * real_p = (char *)p - extra;
    assert(*(size_t *)real_p == n);
    Alloc::deallocate(real_p, n + extra);
}

static void * reallocate(void *p, size_t old_sz, size_t new_sz)
{
    char * real_p = (char *)p - extra;
    assert(*(size_t *)real_p == old_sz);
    char * result = (char *)
                  Alloc::reallocate(real_p, old_sz + extra, new_sz + extra);
    *(size_t *)result = new_sz;
    return result + extra;
}


};


# ifdef __USE_MALLOC

typedef malloc_alloc alloc;
typedef malloc_alloc single_client_alloc;

# else


// Default node allocator.
// With a reasonable compiler, this should be roughly as fast as the
// original STL class-specific allocators, but with less fragmentation.
// Default_alloc_template parameters are experimental and MAY
// DISAPPEAR in the future.  Clients should just use alloc for now.
//
// Important implementation properties:
// 1. If the client request an object of size > __MAX_BYTES, the resulting
//    object will be obtained directly from malloc.
// 2. In all other cases, we allocate an object of size exactly
//    ROUND_UP(requested_size).  Thus the client has enough size
//    information that we can return the object to the proper free list
//    without permanently losing part of the object.
//

// The first template parameter specifies whether more than one thread
// may use this allocator.  It is safe to allocate an object from
// one instance of a default_alloc and deallocate it with another
// one.  This effectively transfers its ownership to the second one.
// This may have undesirable effects on reference locality.
// The second parameter is unreferenced and serves only to allow the
// creation of multiple default_alloc instances.
// Node that containers built on different allocator instances have
// different types, limiting the utility of this approach.
#ifdef __SUNPRO_CC
// breaks if we make these template class members:
  enum {__ALIGN = 8};
  enum {__MAX_BYTES = 128};
  enum {__NFREELISTS = __MAX_BYTES/__ALIGN};
#endif

/*
 * 第二级配置器：默认的内存分配器
 *
 * 特点：完全使用 静态 成员变量和静态成员函数，方便使用C语言重写
 * */
template <bool threads, int inst>
class __default_alloc_template {

private:
  // Really we should use static const int x = N
  // instead of enum { x = N }, but few compilers accept the former.
# ifndef __SUNPRO_CC
    // 常量定义 建议 定义为 constant
    enum {__ALIGN = 8}; // 设置对齐要求. 对齐为8字节, 没有8字节自动补齐
    enum {__MAX_BYTES = 128}; // 第二级配置器的最大一次性申请大小, 大于128就直接调用第一级配置器
    enum {__NFREELISTS = __MAX_BYTES/__ALIGN}; // 128字节能分配的的链表个数, 分别代表8, 16, 32....字节的链表
    static const int NOBJS = 20; // 更友好的编码方式
# endif
  // 将 bytes 上调到 8 的倍数，内存分配都是以 一定字节数的倍数进行分配的，而不是任意字节分配
  // 例如 byte 为 13， 则 （13 + 7）&~ 7 = 16 同时计算后续的追加量
  /*************************************************************/
  static size_t ROUND_UP(size_t bytes) {
        return (((bytes) + __ALIGN-1) & ~(__ALIGN - 1));
  }
__PRIVATE:
  // free-list 节点 构建， 采用 union 使 free_list_link 与 client_data 共享内存
  /*
  union old_obj {
        union old_obj * free_list_link;
        char client_data[1];    // The client sees this. 此部分代码无效 可以删掉
  };
  */

  // 使用 struct 替代
  // embeded pointer 实际上一个单向链表，free_list_link 实际上就是 next 指针
  /*
   obj(node)
   | |-> free_list_link

   * */
  struct obj {
      struct obj * free_list_link;
  };

private:
# ifdef __SUNPRO_CC
    static obj * __VOLATILE free_list[]; 
        // Specifying a size results in duplicate def for 4.1
# else
    // 16 个 free list 节点 声明 的数组，每个元素存储各自链表的头指针 obj*
    static obj * __VOLATILE free_list[__NFREELISTS];
# endif
  // 根据 数据快 bytes 大小，决定使用 第 n 号 链表， 比如 8 则对应 2 号链表
  static  size_t FREELIST_INDEX(size_t bytes) {
        return (((bytes) + __ALIGN-1)/__ALIGN - 1);
  }

  // Returns an object of size n, and optionally adds to size n free list.
  // 链表为空时，就需要填充内存
  static void *refill(size_t n);
  // Allocates a chunk for nobjs of size size.  nobjs may be reduced
  // if it is inconvenient to allocate the requested number.
  // 分配一大块内存
  static char *chunk_alloc(size_t size, int &nobjs);

  // Chunk allocation state.
  // pool 使用 char 声明 是因为 char 是一个字节，方便后续字节计算
  static char *start_free; // 内存池起始位置
  static char *end_free; // 内存池终止位置
  static size_t heap_size; // 分配累计总量，方便计算累计分配量

# ifdef __STL_SGI_THREADS
    static volatile unsigned long __node_allocator_lock;
    static void __lock(volatile unsigned long *); 
    static inline void __unlock(volatile unsigned long *);
# endif

# ifdef _PTHREADS
    static pthread_mutex_t __node_allocator_lock;
# endif

# ifdef __STL_WIN32THREADS
    static CRITICAL_SECTION __node_allocator_lock;
    static bool __node_allocator_lock_initialized;

  public:
    __default_alloc_template() {
	// This assumes the first constructor is called before threads
	// are started.
        if (!__node_allocator_lock_initialized) {
            InitializeCriticalSection(&__node_allocator_lock);
            __node_allocator_lock_initialized = true;
        }
    }
  private:
# endif

    class lock {
        public:
            lock() { __NODE_ALLOCATOR_LOCK; }
            ~lock() { __NODE_ALLOCATOR_UNLOCK; }
    };
    friend class lock;

public:

  /* n must be > 0      */
  static void * allocate(size_t n)
  {
    //  __VOLATILE 多线程关键字
    obj * __VOLATILE * my_free_list; // obj** 一个指针的指针，指向元素为 obj* （指针）的数组
    obj * __RESTRICT result;

    // 先判断申请的字节大小是不是大于128字节, 是, 则交给第一级配置器来处理. 否, 继续往下执行
    if (n > (size_t) __MAX_BYTES) {
        return(malloc_alloc::allocate(n));
    }
    // 找到分配的地址对齐后分配的是第几个大小的链表
    // 头指针 free_list 向后移动 FREELIST_INDEX(n) 达到对应 的链表
    my_free_list = free_list + FREELIST_INDEX(n);
    // Acquire the lock here with a constructor call.
    // This ensures that it is released in exit or during stack
    // unwinding.
#       ifndef _NOTHREADS
        /*REFERENCED*/
        lock lock_instance;
#       endif
    // 获得该链表指向的首地址, 如果链表没有多余的内存, 就重新填充链表
    // 注意这个 首地址是可分配内存的首地址，当前内存块分配出去以后，当前指针更新为下一个可分配区块
    result = *my_free_list;
    if (0 == result) {
        void *r = refill(ROUND_UP(n)); // refill内存填充
        return r;
    }
    // 如果 该链表不为空，则分配 *my_free_list指向 的区块，并将  *my_free_list 指向下一个待分配的区块
    *my_free_list = result -> free_list_link;
    return (result);
  };

  /* p may not be 0 */

  /*****************************************/
  /*
   * 这个函数有两个注意点：
   * 1. 这里只将 内存 回收到 链表中，但是没有释放到操作系统（这不是内存泄露，因为后续还是可以继续分配的）
   * 2. 没有对 p 进行合法性检查，如果 p 不是 8 字节对齐（没有调用 alloc 进行分配），就很危险
   * 这里有一个潜在问题，没有对 p 进行合法性检查
   * */
  static void deallocate(void *p, size_t n)
  {
    obj *q = (obj *)p;
    obj * __VOLATILE * my_free_list;

    // 释放的内存大于128字节直接调用一级配置器进行释放
    if (n > (size_t) __MAX_BYTES) {
        malloc_alloc::deallocate(p, n);
        return;
    }
    // 仍然还是根据字节数要找到对应的链表进行回收
    my_free_list = free_list + FREELIST_INDEX(n);
    // acquire lock
#       ifndef _NOTHREADS
        /*REFERENCED*/
        lock lock_instance;
#       endif /* _NOTHREADS */

    /* q 的 next 指针指向 即将分配的区块指针 *my_free_list，同时将 q 作为链表头存入 链表数组 */
    q -> free_list_link = *my_free_list;
    *my_free_list = q;
    // lock is released here
  }

  static void * reallocate(void *p, size_t old_sz, size_t new_sz);

} ;

typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;
typedef __default_alloc_template<false, 0> single_client_alloc;



/* We allocate memory in large chunks in order to avoid fragmenting     */
/* the malloc heap too much.                                            */
/* We assume that size is properly aligned.                             */
/* We hold the allocation lock.                                         */

/*
 * chunk_alloc 的算法思路：
 *   先检查 pool是否有足够余量，有的话指直接划分，否则，有碎片进行特殊处理，然后分配一大块内存
 *  具体算法逻辑：
 *  1. 内存池的大小大于需要的空间, 直接返回起始地址(nobjs默认设置为20, 所以每次调用都会给链表额外的19个内存块)
 *  2. 内存池的内存不足以马上分配那么多内存, 但是还能满足分配一个即以上的大小, 那就分配出去，知道余量不足一个区块
 *  3. 如果一个对象的大小都已经提供不了了, 先将零碎的内存块给一个小内存的链表来保存, 然后就准备调用malloc申请40块+额外大小的内存块(额外内存块就由heap_size决定), 如果申请失败跳转到步骤4, 成功跳转到步骤6
 *  4. 通过递归来调用他们的内存块，进行内存区块分配检查
 *  5. 如果还是没有内存块, 直接调用一级配置器来申请内存, 还是失败就抛出异常, 成功申请就继续执行
 *  6. 重新修改内存起始地址和结束地址为当前申请的地址块, 重新调用chunk_alloc分配内存
 * */
template <bool threads, int inst>
char*
// int& nobjs pass by reference 的原因是 分配不足 20个 区块时，就需要修改 nobjs
__default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs)
{
    char * result;
    size_t total_bytes = size * nobjs; // 需要的字节数

    /**********************************************/
    /*
     *           |     20    |
     *           |   blocks  |
     *           =============
     *           |           |-----------> start_free
     *           |   pool    |
     *           |___________|-----------> end_free
     *
     * */

    size_t bytes_left = end_free - start_free; // 查看之前的 pool 中还有多少个字节备用

    // pool 中的 size 余量 满足所需 20 个区块大小
    if (bytes_left >= total_bytes) {
        result = start_free;
        start_free += total_bytes; // start 下移
        return(result);
    } else if (bytes_left >= size) {
        // 是否满足 1 个区块大小
        nobjs = bytes_left/size; // 计算能切几个 区块
        total_bytes = size * nobjs; // 基于更新的区块数计算所需总的字节数
        // start 下移
        result = start_free;
        start_free += total_bytes;
        return(result);
    } else {
        /* 不足一个，就需要碎片处理以及大块内存分配 */
        // Try to make use of the left-over piece.
        // 内存碎片处理，将碎片划分到指定 号链表
        if (bytes_left > 0) {
            // 剩余碎片一定还是 8 对齐，找到对应链表号，直接加入他的区块
            obj * __VOLATILE * my_free_list =
                        free_list + FREELIST_INDEX(bytes_left);

            // cur->next = head ; head = cur;
            ((obj *)start_free) -> free_list_link = *my_free_list;
            *my_free_list = (obj *)start_free;
        }

        // 计算 需要大块内存的分配量
        /*
        * 分配 2 * 20 个块 + 一个 heap_size（累计分配内存总量的） 追加量
        * 除了 20 个块 作为区块 分配对应链表，其他都作为 pool 内存
         *  这里进行了调整 保证 bytes_to_get 被定义初始化就被立即使用
         *  防止中间变量出现遗漏的问题
        * */
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        // 使用 malloc 进行内存分配
        start_free = (char *)malloc(bytes_to_get);

        // 人为限制 分配量不准超过 10000 用于模拟分配失败
//        if (bytes_to_get > 10000)
//            start_free = 0;

        if (0 == start_free) {
            // 分配失败，会向右边更大区块的链表进行内存分配
            int i;

            // 注意这里的类型定义等价
            // obj * __VOLATILE * my_free_list, *p;
            obj* __VOLATILE * my_free_list;
            obj* p;

            // Try to make do with what we have.  That can't
            // hurt.  We do not try smaller requests, since that tends
            // to result in disaster on multi-process machines.
            for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                /* 这种写法的好处 防止 p = 0 的bug，编译器无法查看 */
                if (0 != p) {
                    // 如果右边能获得区块，就将需要的大小区块直接划进 pool
                    *my_free_list = p -> free_list_link;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    return(chunk_alloc(size, nobjs));
                    // Any leftover piece will eventually make it to the
                    // right free list.
                }
            }
	        end_free = 0;	// In case of exception.
            // 如果一点内存都没有了的话, 就只有调用一级配置器来申请内存了, 并且用户没有设置处理例程就抛出异常
            start_free = (char *)malloc_alloc::allocate(bytes_to_get);
            // This should either throw an
            // exception or remedy the situation.  Thus we assume it
            // succeeded.
        }

        // 分配成功
        heap_size += bytes_to_get; // 更新分配量
        end_free = start_free + bytes_to_get;

        // 递归检查，主要是可能因为 系统内存问题，仍然不够 20 个，就递归下去，进行分块
        return(chunk_alloc(size, nobjs));
    }
}


/* Returns an object of size n, and optionally adds to size n free list.*/
/* We assume that n is properly aligned.                                */
/* We hold the allocation lock.                                         */
template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t n)
{
    // 默认 分配 20 个区块作为链表的带分配内存，各个块之间通过 指针 链接
    // 但是 未必能够 分配够 20 个，这里建议 设置为 const 声明！
    int nobjs = NOBJS;
    // 向内存池申请空间的起始地址
    char * chunk = chunk_alloc(n, nobjs); // pass by reference
    obj * __VOLATILE * my_free_list;
    obj * result;
    obj * current_obj, * next_obj;
    int i;

    // 如果只申请到一个对象的大小, 就直接返回一个内存的大小，不需要切割
    if (1 == nobjs) return(chunk);

    // 申请的内存不止一个区块时，就要进行切割，块于块之间通过链表链接
    my_free_list = free_list + FREELIST_INDEX(n);

    // 第一个区块不要划分，是要给出去的，所以直接划分后面的大块，首个 指针作为 结果输出
    result = (obj *)chunk;

    /* Build free list in chunk 在 chunk 里面建立自由链表*/
    // my_free_list指向内存池返回的地址的下一个对齐后的地址 由于 指针是 char* 所以直接 +n 就是一个第二个区块的地址
      *my_free_list = next_obj = (obj *)(chunk + n);
    // 这里从第二个开始的原因主要是第一块地址返回给了用户, 现在需要把从内存池里面分配的区块用链表给串起来
      for (i = 1; ; i++) {
        current_obj = next_obj; // cur
        next_obj = (obj *)((char *)next_obj + n); // 指针移动 n 个字节 获取 next
        if (nobjs - 1 == i) {
            current_obj -> free_list_link = 0;
            break;
        } else {
            current_obj -> free_list_link = next_obj; // cur->next = next
        }
      }

    return(result);
}

template <bool threads, int inst>
void*
__default_alloc_template<threads, inst>::reallocate(void *p,
                                                    size_t old_sz,
                                                    size_t new_sz)
{
    void * result;
    size_t copy_sz;

    if (old_sz > (size_t) __MAX_BYTES && new_sz > (size_t) __MAX_BYTES) {
        return(realloc(p, new_sz));
    }
    if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return(p);
    result = allocate(new_sz);
    copy_sz = new_sz > old_sz? old_sz : new_sz;
    memcpy(result, p, copy_sz);
    deallocate(p, old_sz);
    return(result);
}

#ifdef _PTHREADS
    template <bool threads, int inst>
    pthread_mutex_t
    __default_alloc_template<threads, inst>::__node_allocator_lock
        = PTHREAD_MUTEX_INITIALIZER;
#endif

#ifdef __STL_WIN32THREADS
    template <bool threads, int inst> CRITICAL_SECTION
    __default_alloc_template<threads, inst>::__node_allocator_lock;

    template <bool threads, int inst> bool
    __default_alloc_template<threads, inst>::__node_allocator_lock_initialized
	= false;
#endif

#ifdef __STL_SGI_THREADS
__STL_END_NAMESPACE
#include <mutex.h>
#include <time.h>
__STL_BEGIN_NAMESPACE
// Somewhat generic lock implementations.  We need only test-and-set
// and some way to sleep.  These should work with both SGI pthreads
// and sproc threads.  They may be useful on other systems.
template <bool threads, int inst>
volatile unsigned long
__default_alloc_template<threads, inst>::__node_allocator_lock = 0;

#if __mips < 3 || !(defined (_ABIN32) || defined(_ABI64)) || defined(__GNUC__)
#   define __test_and_set(l,v) test_and_set(l,v)
#endif

template <bool threads, int inst>
void 
__default_alloc_template<threads, inst>::__lock(volatile unsigned long *lock)
{
    const unsigned low_spin_max = 30;  // spin cycles if we suspect uniprocessor
    const unsigned high_spin_max = 1000; // spin cycles for multiprocessor
    static unsigned spin_max = low_spin_max;
    unsigned my_spin_max;
    static unsigned last_spins = 0;
    unsigned my_last_spins;
    static struct timespec ts = {0, 1000};
    unsigned junk;
#   define __ALLOC_PAUSE junk *= junk; junk *= junk; junk *= junk; junk *= junk
    int i;

    if (!__test_and_set((unsigned long *)lock, 1)) {
        return;
    }
    my_spin_max = spin_max;
    my_last_spins = last_spins;
    for (i = 0; i < my_spin_max; i++) {
        if (i < my_last_spins/2 || *lock) {
            __ALLOC_PAUSE;
            continue;
        }
        if (!__test_and_set((unsigned long *)lock, 1)) {
            // got it!
            // Spinning worked.  Thus we're probably not being scheduled
            // against the other process with which we were contending.
            // Thus it makes sense to spin longer the next time.
            last_spins = i;
            spin_max = high_spin_max;
            return;
        }
    }
    // We are probably being scheduled against the other process.  Sleep.
    spin_max = low_spin_max;
    for (;;) {
        if (!__test_and_set((unsigned long *)lock, 1)) {
            return;
        }
        nanosleep(&ts, 0);
    }
}

template <bool threads, int inst>
inline void
__default_alloc_template<threads, inst>::__unlock(volatile unsigned long *lock)
{
#   if defined(__GNUC__) && __mips >= 3
        asm("sync");
        *lock = 0;
#   elif __mips >= 3 && (defined (_ABIN32) || defined(_ABI64))
        __lock_release(lock);
#   else 
        *lock = 0;
        // This is not sufficient on many multiprocessors, since
        // writes to protected variables and the lock may be reordered.
#   endif
}
#endif

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = 0;  // 内存池的首地址

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = 0;  // 内存池的结束地址

template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj * __VOLATILE
__default_alloc_template<threads, inst> ::free_list[
# ifdef __SUNPRO_CC
    __NFREELISTS
# else
    __default_alloc_template<threads, inst>::__NFREELISTS
# endif
] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
// The 16 zeros are necessary to make version 4.1 of the SunPro
// compiler happy.  Otherwise it appears to allocate too little
// space for the array.

# ifdef __STL_WIN32THREADS
  // Create one to get critical section initialized.
  // We do this onece per file, but only the first constructor
  // does anything.
  static alloc __node_allocator_dummy_instance;
# endif

#endif /* ! __USE_MALLOC */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif

__STL_END_NAMESPACE

#undef __PRIVATE

#endif /* __SGI_STL_INTERNAL_ALLOC_H */

// Local Variables:
// mode:C++
// End:
