// Loki 侯捷調整版
// 檔名 : TypeList-jj.h
// 根據 : Loki's TypeList.h
// 調整者 : jjhou
// 最後更改日期 : 20030402
// 說明：本檔僅為個人修練之用
// 適用：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1)  
//       GNU-C++ 2.95.3-5 (Cygwin special,20010316)
// 編譯：command line mode 直接以 g++ 編譯，不需任何選項（options）

// g32[o] 
// g291[o]（但 GenScatterHierarchy's object 不通過）
// cb6[x] cb5[x] 
//   partial specialization is not so GOOD in CB6 & CB5

// p53 Length 		OK
// p55 TypeAt		OK
// p56 IndexOf		OK
// p57 Append		OK
// p58 Erase		OK
// p59 EraseAll		OK
// p59 NoDuplicates	OK
// p60 Replace		OK
// p61 ReplaceAll	OK
// p61 DerivedToFront
// p63 MostDerived
// p65 GenScatterHierarchy	OK
// p71 GenLinearHierarchy	OK


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

#ifndef TYPELIST_INC_
#define TYPELIST_INC_

#include "NullType-jj.h"
#include "EmptyType-jj.h"

namespace Loki
{
    namespace TL
    {
        template <class T, class U>
        struct Typelist
        {
            typedef T Head;
            typedef U Tail;
        };

        #define TYPELIST_1(T1) Typelist<T1, NullType>
        #define TYPELIST_2(T1, T2) Typelist<T1, TYPELIST_1(T2) >
        #define TYPELIST_3(T1, T2, T3) Typelist<T1, TYPELIST_2(T2, T3) >
        #define TYPELIST_4(T1, T2, T3, T4) Typelist<T1, TYPELIST_3(T2, T3, T4) >

        // Length, p53
        template <class TList>
        struct Length;
        template <>
        struct Length<NullType>
        {
            enum { value = 0 };
        };
        template <class T, class U>
        struct Length< Typelist<T, U> >
        {
            enum { value = 1 + Length<U>::value };
        };

        // TypeAt, p55
        template <class TList, unsigned int index>
        struct TypeAt;
        template <class Head, class Tail>
        struct TypeAt<Typelist<Head, Tail>, 0>
        {
            typedef Head Result;
        };
        template <class Head, class Tail, unsigned int i>
        struct TypeAt<Typelist<Head, Tail>, i>
        {
            typedef typename TypeAt<Tail, i-1>::Result Result;
        };

        // TypeAtNonStrict, p55 (copy from <Typelist.h>
        template <class TList, unsigned int index,
            typename DefaultType = NullType>
        struct TypeAtNonStrict
        {
            typedef DefaultType Result;
        };        
        template <class Head, class Tail, typename DefaultType>
        struct TypeAtNonStrict<Typelist<Head, Tail>, 0, DefaultType>
        {
            typedef Head Result;
        };        
        template <class Head, class Tail, unsigned int i, typename DefaultType>
        struct TypeAtNonStrict<Typelist<Head, Tail>, i, DefaultType>
        {
            typedef typename 
                TypeAtNonStrict<Tail, i - 1, DefaultType>::Result Result;
        };    

        // IndexOf, p56
        template <class TList, class T>
        struct IndexOf;
        template <class T>
        struct IndexOf<NullType, T>
        {
            enum { value = -1 };
        };
        template <class Tail, class T>
        struct IndexOf<Typelist<T, Tail>, T>
        {
            enum { value = 0 };
        };
        template <class Head, class Tail, class T>
        struct IndexOf<Typelist<Head, Tail>, T>
        {
        private:
                enum { temp = IndexOf<Tail, T>::value };
        public:
                enum { value = temp == -1 ? -1 : 1 + temp };
        };

        // Append, p57
        template <class TList, class T>
        struct Append;
        template <>
        struct Append<NullType, NullType>
        {
            typedef NullType Result;
        };
        template <class T>
        struct Append<NullType, T>
        {
            typedef TYPELIST_1(T) Result;
        };
        template <class Head, class Tail>
        struct Append<NullType, Typelist<Head, Tail> >
        {
            typedef Typelist<Head, Tail> Result;
        };
        template <class Head, class Tail, class T>
        struct Append<Typelist<Head, Tail>, T>
        {
            typedef Typelist<Head,
                    typename Append<Tail, T>::Result>
                Result;
        };

        // Erase, p58
        template <class TList, class T>
        struct Erase;
        template <class T>                            // Specialization 1
        struct Erase<NullType, T>
        {
            typedef NullType Result;
        };
        template <class T, class Tail>                // Specialization 2
        struct Erase<Typelist<T, Tail>, T>
        {
            typedef Tail Result;
        };
        template <class Head, class Tail, class T>    // Specialization 3
        struct Erase<Typelist<Head, Tail>, T>
        {
            typedef Typelist<Head,
                    typename Erase<Tail, T>::Result>
                Result;
        };

        // EraseAll, p59
        template <class TList, class T>
        struct EraseAll;
        template <class T>
        struct EraseAll<NullType, T>
        {
            typedef NullType Result;
        };
        template <class T, class Tail>
        struct EraseAll<Typelist<T, Tail>, T>
        {
            // Go all the way down the list removing the type
            typedef typename EraseAll<Tail, T>::Result Result;
        };
        template <class Head, class Tail, class T>
        struct EraseAll<Typelist<Head, Tail>, T>
        {
            // Go all the way down the list removing the type
            typedef Typelist<Head,
                    typename EraseAll<Tail, T>::Result>
                Result;
        };

        // NoDuplicates, p59
        template <class TList>
        struct NoDuplicates;
        template <>
        struct NoDuplicates<NullType>
        {
            typedef NullType Result;
        };
        template <class Head, class Tail>
        struct NoDuplicates< Typelist<Head, Tail> >
        {
        private:
            typedef typename NoDuplicates<Tail>::Result L1;
            typedef typename Erase<L1, Head>::Result L2;
        public:
            typedef Typelist<Head, L2> Result;
        };

        // Replace, p60
        template <class TList, class T, class U>
        struct Replace;
        template <class T, class U>
        struct Replace<NullType, T, U>
        {
            typedef NullType Result;
        };
        template <class T, class Tail, class U>
        struct Replace<Typelist<T, Tail>, T, U>
        {
            typedef Typelist<U, Tail> Result;
        };
        template <class Head, class Tail, class T, class U>
        struct Replace<Typelist<Head, Tail>, T, U>
        {
            typedef Typelist<Head,
                    typename Replace<Tail, T, U>::Result>
                Result;
        };

        // ReplaceAll, p61
        template <class TList, class T, class U>
        struct ReplaceAll;
        template <class T, class U>
        struct ReplaceAll<NullType, T, U>
        {
            typedef NullType Result;
        };
        template <class T, class Tail, class U>
        struct ReplaceAll<Typelist<T, Tail>, T, U>
        {
            typedef Typelist<U, typename ReplaceAll<Tail, T, U>::Result> Result;
        };
        template <class Head, class Tail, class T, class U>
        struct ReplaceAll<Typelist<Head, Tail>, T, U>
        {
            typedef Typelist<Head,
                    typename ReplaceAll<Tail, T, U>::Result>
                Result;
        };
        
        // ReplaceAll, p61
        template <class TList> 
        struct Reverse;
        template <class T>
        struct Reverse< TYPELIST_1(T) >
        {
            typedef TYPELIST_1(T) Result;
        };
        template <class Head, class Tail>
        struct Reverse< Typelist<Head, Tail> >
        {
            typedef typename Append<
                typename Reverse<Tail>::Result, Head>::Result Result;
        };


        // GenScatterHierarchy, p65
        // copy from HierarchyGenerators.h
        template <class TList, template <class> class Unit>
        class GenScatterHierarchy;
        
        template <class T1, class T2, template <class> class Unit>
        class GenScatterHierarchy<Typelist<T1, T2>, Unit>
            : public GenScatterHierarchy<T1, Unit>
            , public GenScatterHierarchy<T2, Unit>
        {
        public:
            typedef Typelist<T1, T2> TList;
            typedef GenScatterHierarchy<T1, Unit> LeftBase;
            typedef GenScatterHierarchy<T2, Unit> RightBase;

            template <class T>
            struct Rebind
            { typedef Unit<T> Result; };
        };

        template <class AtomicType, template <class> class Unit>
        class GenScatterHierarchy : public Unit<AtomicType>
        {
           typedef Unit<AtomicType> LeftBase;

           template <class T>
           struct Rebind
           { typedef Unit<T> Result; };
        };

        template <template <class> class Unit>
        class GenScatterHierarchy<NullType, Unit>
        {
           template <class T>
           struct Rebind
           { typedef Unit<T> Result; };
        };
        
        
    // GenLinearHierarchy, p72
    // copy from HierarchyGenerators.h        
    template
    <
        class TList,
        template <class AtomicType, class Base> class Unit,
        class Root = EmptyType
    >
    class GenLinearHierarchy;
    
    template
    <
        class T1,
        class T2,
        template <class, class> class Unit,
        class Root
    >
    class GenLinearHierarchy<Typelist<T1, T2>, Unit, Root>
        : public Unit< T1, GenLinearHierarchy<T2, Unit, Root> >
    {    };

    template
    <
        class T,
        template <class, class> class Unit,
        class Root
    >
    class GenLinearHierarchy<TYPELIST_1(T), Unit, Root>
        : public Unit<T, Root>
    {    };
    
    
    
    } // namespace TL
} // namespace Loki

#endif // TYPELIST_INC_
