// Loki 侯捷調整版
// 檔名 : TypeTraits-jj.h
// 根據 : Loki's TypeTraits.h
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


#ifndef TYPETRAITS_INC_
#define TYPETRAITS_INC_

#include "TypeList-jj.h"
// 我在很多 typedef 地方加上 typename. 否則 g32[w] 

namespace Loki
{
   using namespace TL; // jjhou add
   	
////////////////////////////////////////////////////////////////////////////////
// class template IsCustomUnsignedInt
// Offers a means to integrate nonstandard built-in unsigned integral types
// (such as unsigned __int64 or unsigned long long int) with the TypeTraits 
//     class template defined below.
// Invocation: IsCustomUnsignedInt<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in unsigned
//     integral type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomUnsignedInt
    {
        enum { value = 0 };
    };        

////////////////////////////////////////////////////////////////////////////////
// class template IsCustomSignedInt
// Offers a means to integrate nonstandard built-in unsigned integral types
// (such as unsigned __int64 or unsigned long long int) with the TypeTraits 
//     class template defined below.
// Invocation: IsCustomSignedInt<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in signed
//     integral type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomSignedInt
    {
        enum { value = 0 };
    };        

////////////////////////////////////////////////////////////////////////////////
// class template IsCustomFloat
// Offers a means to integrate nonstandard floating point types with the
//     TypeTraits class template defined below.
// Invocation: IsCustomFloat<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in
//     floating point type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomFloat
    {
        enum { value = 0 };
    };        

////////////////////////////////////////////////////////////////////////////////
// Helper types for class template TypeTraits defined below
////////////////////////////////////////////////////////////////////////////////
    
    namespace Private
    {
        typedef TYPELIST_4(unsigned char, unsigned short int,
           unsigned int, unsigned long int) StdUnsignedInts;
        typedef TYPELIST_4(signed char, short int,
           int, long int) StdSignedInts;
        typedef TYPELIST_3(bool, char, wchar_t) StdOtherInts;
        typedef TYPELIST_3(float, double, long double) StdFloats;
    }
      
////////////////////////////////////////////////////////////////////////////////
// class template TypeTraits
// Figures out various properties of any given type
// Invocations (T is a type):
// a) TypeTraits<T>::isPointer
// returns (at compile time) true if T is a pointer type
// b) TypeTraits<T>::PointeeType
// returns the type to which T points is T is a pointer type, NullType otherwise
// a) TypeTraits<T>::isReference
// returns (at compile time) true if T is a reference type
// b) TypeTraits<T>::ReferredType
// returns the type to which T refers is T is a reference type, NullType
// otherwise
// c) TypeTraits<T>::isMemberPointer
// returns (at compile time) true if T is a pointer to member type
// d) TypeTraits<T>::isStdUnsignedInt
// returns (at compile time) true if T is a standard unsigned integral type
// e) TypeTraits<T>::isStdSignedInt
// returns (at compile time) true if T is a standard signed integral type
// f) TypeTraits<T>::isStdIntegral
// returns (at compile time) true if T is a standard integral type
// g) TypeTraits<T>::isStdFloat
// returns (at compile time) true if T is a standard floating-point type
// h) TypeTraits<T>::isStdArith
// returns (at compile time) true if T is a standard arithmetic type
// i) TypeTraits<T>::isStdFundamental
// returns (at compile time) true if T is a standard fundamental type
// j) TypeTraits<T>::isUnsignedInt
// returns (at compile time) true if T is a unsigned integral type
// k) TypeTraits<T>::isSignedInt
// returns (at compile time) true if T is a signed integral type
// l) TypeTraits<T>::isIntegral
// returns (at compile time) true if T is a integral type
// m) TypeTraits<T>::isFloat
// returns (at compile time) true if T is a floating-point type
// n) TypeTraits<T>::isArith
// returns (at compile time) true if T is a arithmetic type
// o) TypeTraits<T>::isFundamental
// returns (at compile time) true if T is a fundamental type
// p) TypeTraits<T>::ParameterType
// returns the optimal type to be used as a parameter for functions that take Ts
// q) TypeTraits<T>::isConst
// returns (at compile time) true if T is a const-qualified type
// r) TypeTraits<T>::NonConstType
// removes the 'const' qualifier from T, if any
// s) TypeTraits<T>::isVolatile
// returns (at compile time) true if T is a volatile-qualified type
// t) TypeTraits<T>::NonVolatileType
// removes the 'volatile' qualifier from T, if any
// u) TypeTraits<T>::UnqualifiedType
// removes both the 'const' and 'volatile' qualifiers from T, if any
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    class TypeTraits
    {
    private:
        template <class U> struct PointerTraits
        {
            enum { result = false };
            typedef NullType PointeeType;
        };
        
        template <class U> struct PointerTraits<U*>
        {
            enum { result = true };
            typedef U PointeeType;
        };
        
        template <class U> struct ReferenceTraits
        {
            enum { result = false };
            typedef U ReferredType;
        };
        
        template <class U> struct ReferenceTraits<U&>
        {
            enum { result = true };
            typedef U ReferredType;
        };
        
        template <class U> struct PToMTraits
        {
            enum { result = false };
        };
        
        template <class U, class V>
        struct PToMTraits<U V::*>
        {
            enum { result = true };
        };

        template <class U> struct UnConst
        {
            typedef U Result;
            enum { isConst = 0 };
        };
        
        template <class U> struct UnConst<const U>
        {
            typedef U Result;
            enum { isConst = 1 };
        };

        template <class U> struct UnVolatile
        {
            typedef U Result;
            enum { isVolatile = 0 };
        };
        
        template <class U> struct UnVolatile<volatile U>
        {
            typedef U Result;
            enum { isVolatile = 1 };
        };

    public:
        enum { isPointer = PointerTraits<T>::result };
        typedef typename PointerTraits<T>::PointeeType PointeeType;

        enum { isReference = ReferenceTraits<T>::result };
        typedef typename ReferenceTraits<T>::ReferredType ReferredType;
        
        enum { isMemberPointer = PToMTraits<T>::result };
/*    
// 以下和先前被 mark 起來的 4 種 types 有關，所以一併 remark.
        enum { isStdUnsignedInt = 
            TL::IndexOf<Private::StdUnsignedInts, T>::value >= 0 };
        enum { isStdSignedInt = 
            TL::IndexOf<Private::StdSignedInts, T>::value >= 0 };
        enum { isStdIntegral = isStdUnsignedInt || isStdSignedInt ||
            TL::IndexOf<Private::StdOtherInts, T>::value >= 0 };
        enum { isStdFloat = TL::IndexOf<Private::StdFloats, T>::value >= 0 };
        enum { isStdArith = isStdIntegral || isStdFloat };
        enum { isStdFundamental = isStdArith || isStdFloat || 
            Conversion<T, void>::sameType };
            
        enum { isUnsignedInt = isStdUnsignedInt || IsCustomUnsignedInt<T>::value };
        enum { isSignedInt = isStdSignedInt || IsCustomSignedInt<T>::value };
        enum { isIntegral = isStdIntegral || isUnsignedInt || isSignedInt };
        enum { isFloat = isStdFloat || IsCustomFloat<T>::value };
        enum { isArith = isIntegral || isFloat };
        enum { isFundamental = isStdFundamental || isArith || isFloat };
        
        typedef typename Select<isStdArith || isPointer || isMemberPointer,
                T, ReferredType&>::Result
            ParameterType;
*/        
        enum { isConst = UnConst<T>::isConst };
        typedef typename UnConst<T>::Result NonConstType;
        enum { isVolatile = UnVolatile<T>::isVolatile };
        typedef typename UnVolatile<T>::Result NonVolatileType;
        typedef typename UnVolatile<typename UnConst<T>::Result>::Result UnqualifiedType;
    };
}

#endif // TYPETRAITS_INC_
