// Loki 侯捷調整版
// 檔名 : Factory-jj.h
// 根據 : Loki's Factory.h
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


#ifndef FACTORY_INC_
#define FACTORY_INC_

#include "TypeInfo-jj.h"
#include <exception>
#include <map>

namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
// class template DefaultFactoryError
// Manages the "Unknown Type" error in an object factory
////////////////////////////////////////////////////////////////////////////////

    template <typename IdentifierType, class AbstractProduct>
    struct DefaultFactoryError
    {
        struct Exception : public std::exception
        {
            const char* what() const { return "Unknown Type"; }
        };
        
        static AbstractProduct* OnUnknownType(IdentifierType)
        {
            throw Exception();
        }
    };

////////////////////////////////////////////////////////////////////////////////
// class template Factory
// Implements a generic object factory
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class AbstractProduct, 
        typename IdentifierType,
        typename ProductCreator = AbstractProduct* (*)()
        // template<typename, class>
        //    class FactoryErrorPolicy = DefaultFactoryError
    >
    class Factory 
        // : public FactoryErrorPolicy<IdentifierType, AbstractProduct>
    {
    public:
        bool Register(const IdentifierType& id, ProductCreator creator)
        {
            return associations_.insert(
                IdToProductMap::value_type(id, creator)).second;
        }
        
        bool Unregister(const IdentifierType& id)
        {
            return associations_.erase(id) == 1;
        }
        
        AbstractProduct* CreateObject(const IdentifierType& id)
        {
          // typename IdToProductMap::const_iterator i = associations_.find(id);
            //原本代碼如上。但 const_iterator 會造成稍後呼叫 (i->second)() 時
            //的 this 多出一個 const qualifier，造成該呼叫動作與
            //Factory 的 3rd template 參數 'ProductCreator' 略有差異，
            //導致編譯失敗。
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
            {
                return (i->second)();
            }
            // return OnUnknownType(id);
        }
        
    private:
        typedef std::map<IdentifierType, ProductCreator>  IdToProductMap;
        IdToProductMap associations_;
    };

////////////////////////////////////////////////////////////////////////////////
// class template CloneFactory
// Implements a generic cloning factory
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class AbstractProduct, 
        class ProductCreator = 
            AbstractProduct* (*)(const AbstractProduct*)
        // template<typename, class>
        //    class FactoryErrorPolicy = DefaultFactoryError
    >
    class CloneFactory
        // : public FactoryErrorPolicy<TypeInfo, AbstractProduct>
    {
    public:
        bool Register(const TypeInfo& ti, ProductCreator creator)
        {
            return associations_.insert(
                IdToProductMap::value_type(ti, creator)).second;
        }
        
        bool Unregister(const TypeInfo& id)
        {
            return associations_.erase(id) == 1;
        }
        
        AbstractProduct* CreateObject(const AbstractProduct* model)
        {
            if (model == 0) return 0;
            
            typename IdToProductMap::const_iterator i = 
                associations_.find(typeid(*model));
            if (i != associations_.end())
            {
                return (i->second)(model);
            }
            // return OnUnknownType(typeid(*model));
        }
        
    private:
        typedef std::map<TypeInfo, ProductCreator> IdToProductMap;
        IdToProductMap associations_;
    };
  
} // namespace Loki

#endif // FACTORY_INC_
