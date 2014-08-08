// Loki 侯捷調整版
// 檔名 : Threads-jj.h
// 根據 : Loki's Threads.h
// 調整者 : jjhou
// 最後更改日期 : 20030402
// 說明：本檔僅為個人修練之用
// 適用：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1)  
//       GNU-C++ 2.95.3-5 (Cygwin special,20010316)
// 編譯：command line mode 直接以 g++ 編譯，不需任何選項（options）


////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: February 19, 2001

#ifndef THREADS_INC_
#define THREADS_INC_

////////////////////////////////////////////////////////////////////////////////
// macro DEFAULT_THREADING
// Selects the default threading model for certain components of Loki
// If you don't define it, it defaults to single-threaded
// All classes in Loki have configurable threading model; DEFAULT_THREADING
// affects only default template arguments
////////////////////////////////////////////////////////////////////////////////

#ifndef DEFAULT_THREADING
#define DEFAULT_THREADING /**/ ::Loki::SingleThreaded
#endif

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template SingleThreaded
// Implementation of the ThreadingModel policy used by various classes
// Implements a single-threaded model; no synchronization
////////////////////////////////////////////////////////////////////////////////

    template <class Host>
    class SingleThreaded
    {
    public:
        struct Lock
        {
            Lock() {}
            Lock(const Host&) {}
        };
        
        typedef Host VolatileType;

        typedef int IntType; 

        static IntType AtomicAdd(volatile IntType& lval, IntType val)
        { return lval += val; }
        
        static IntType AtomicSubtract(volatile IntType& lval, IntType val)
        { return lval -= val; }

        static IntType AtomicMultiply(volatile IntType& lval, IntType val)
        { return lval *= val; }
        
        static IntType AtomicDivide(volatile IntType& lval, IntType val)
        { return lval /= val; }
        
        static IntType AtomicIncrement(volatile IntType& lval)
        { return ++lval; }
        
        static IntType AtomicDivide(volatile IntType& lval)
        { return --lval; }
        
        static void AtomicAssign(volatile IntType & lval, IntType val)
        { lval = val; }
        
        static void AtomicAssign(IntType & lval, volatile IntType & val)
        { lval = val; }
    };
    
#ifdef _WINDOWS_

////////////////////////////////////////////////////////////////////////////////
// class template ObjectLevelLockable
// Implementation of the ThreadingModel policy used by various classes
// Implements a object-level locking scheme
////////////////////////////////////////////////////////////////////////////////

#endif // _WINDOWS_

}  // namespace Loki

#endif // THREADS_INC_
