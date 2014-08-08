// Loki 侯捷調整版
// 檔名 : TypeManip-jj.h
// 根據 : Loki's TypeManip.h
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


#ifndef TYPEMANIP_INC_
#define TYPEMANIP_INC_

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template Int2Type
// Converts each integral constant into a unique type
// Invocation: Int2Type<v> where v is a compile-time constant integral
// Defines 'value', an enum that evaluates to v
////////////////////////////////////////////////////////////////////////////////

    template <int v>
    struct Int2Type
    {
        enum { value = v };
    };

////////////////////////////////////////////////////////////////////////////////
// class template Type2Type
// Converts each type into a unique, insipid type
// Invocation Type2Type<T> where T is a type
// Defines the type OriginalType which maps back to T
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct Type2Type
    {
        typedef T OriginalType;
    };

////////////////////////////////////////////////////////////////////////////////
// class template Select
// Selects one of two types based upon a boolean constant
// Invocation: Select<flag, T, U>::Result
// where:
// flag is a compile-time boolean constant
// T and U are types
// Result evaluates to T if flag is true, and to U otherwise.
////////////////////////////////////////////////////////////////////////////////

    template <bool flag, typename T, typename U>
    struct Select
    {
        typedef T Result;
    };
    template <typename T, typename U>
    struct Select<false, T, U>
    {
        typedef U Result;
    };

////////////////////////////////////////////////////////////////////////////////
// Helper types Small and Big - guarantee that sizeof(Small) < sizeof(Big)
////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        typedef char Small;
        class Big { char dummy[2]; };
    }

////////////////////////////////////////////////////////////////////////////////
// class template Conversion
// Figures out the conversion relationships between two types
// Invocations (T and U are types):
// a) Conversion<T, U>::exists
// returns (at compile time) true if there is an implicit conversion from T
// to U (example: Derived to Base)
// b) Conversion<T, U>::exists2Way
// returns (at compile time) true if there are both conversions from T
// to U and from U to T (example: int to char and back)
// c) Conversion<T, U>::sameType
// returns (at compile time) true if T and U represent the same type
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

    template <class T, class U>
    class Conversion
    {
    // jjhou
    public:
        // jjhou
        // static Private::Big Test(...);
        static Private::Big Test(...) { cout << "Test(...)" << endl; return Private::Big(); }  // not be invoked in sizeof()

        // jjhou
        // static Private::Small Test(U);
        static Private::Small Test(U) { cout << "Test(U)" << endl; return Private::Small(); }  // not be invoked in sizeof()

        // jjhou
        // static T MakeT();
        static T MakeT() { cout << "MakeT()" << endl; return *(new T); }   // not be invoked in sizeof()


    public:
        enum { exists = sizeof(Test(MakeT())) == sizeof(Private::Small) };
        // jjhou: 上面這行 cb5[x]：Internal compiler error。
        //                 cb6[o]
        enum { exists2Way = exists &&
            Conversion<U, T>::exists };
        enum { sameType = false };

        // jjhou 添加...
        static bool Exists()
        {
           cout << sizeof(Test(MakeT()));
           cout << sizeof(Private::Small);
           cout << (sizeof(Test(MakeT())) == sizeof(Private::Small)) << ' ';
           cout << exists << "   ";
           // 下面這個式子完全相同於上面的 exists 的計算式
           return  sizeof(Test(MakeT())) == sizeof(Private::Small);
        }
    };

    template <class T>
    class Conversion<T, T>
    {
    public:
        enum { exists = 1, exists2Way = 1,sameType = 1 };
    };

    template <class T>
    class Conversion<void, T>
    {
    public:
        enum { exists = 1, exists2Way = 0,sameType = 0 };
    };

    template <class T>
    class Conversion<T, void>
    {
    public:
        enum { exists = 1, exists2Way = 0,sameType = 0 };
    };

    template <>
    class Conversion<void, void>
    {
    public:
        enum { exists = 1, exists2Way = 1,sameType = 1 };
    };
}

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS
// Invocation: SUPERSUBCLASS(B, D) where B and D are types.
// Returns true if B is a public base of D, or if B and D are aliases of the
// same type.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

#define SUPERSUBCLASS(T, U) \
    (::Loki::Conversion<const U*, const T*>::exists && \
    !::Loki::Conversion<const T*, const void*>::sameType)

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS
// Invocation: SUPERSUBCLASS(B, D) where B and D are types.
// Returns true if B is a public base of D.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

#define SUPERSUBCLASS_STRICT(T, U) \
    (SUPERSUBCLASS(T, U) && \
    !::Loki::Conversion<const T, const U>::sameType)


#endif // TYPEMANIP_INC_
