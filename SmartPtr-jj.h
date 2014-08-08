// Loki 侯捷調整版
// 檔名 : SmartPtr-jj.h
// 根據 : Loki's SmartPtr.h
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

#ifndef SMARTPTR_INC_
#define SMARTPTR_INC_

#include "SmallObj-jj.cpp" // 為求測試方便及簡化的權宜之計，其中含入 "SmallObj-jj.h"
    // 本應含入 "SmallObj-jj.h" 並與 SmallObj-jj.obj (來自 SmallObj-jj.cpp) 聯結
#include "TypeManip-jj.h"
//#include "static_check.h"
#include <functional>   // jjhou: for std:swap 
#include <stdexcept>	// jjhou: for std::runtime_error
#include <iostream>     // jjhou: for std::cout, std::endl

namespace Loki
{
 	
////////////////////////////////////////////////////////////////////////////////
// class template DefaultSPStorage
// Implementation of the StoragePolicy used by SmartPtr
////////////////////////////////////////////////////////////////////////////////
    // copy from <SmartPtr.h>
    template <class T>
    class DefaultSPStorage
    {
    protected:
        typedef T* StoredType;    // the type of the pointee_ object
        typedef T* PointerType;   // type returned by operator->
        typedef T& ReferenceType; // type returned by operator*
        
    public:
        DefaultSPStorage() : pointee_(Default()) 
        {}

        // The storage policy doesn't initialize the stored pointer 
        //     which will be initialized by the OwnershipPolicy's Clone fn
        DefaultSPStorage(const DefaultSPStorage&)
        {}

        template <class U>
        DefaultSPStorage(const DefaultSPStorage<U>&) 
        {}
        
        DefaultSPStorage(const StoredType& p) : pointee_(p) {}
        
        PointerType operator->() const { return pointee_; }
        
        ReferenceType operator*() const { return *pointee_; }
        
        void Swap(DefaultSPStorage& rhs)
        { std::swap(pointee_, rhs.pointee_); }
   
        // Accessors
        friend inline PointerType GetImpl(const DefaultSPStorage& sp)
        { return sp.pointee_; }
        
    	friend inline const StoredType& GetImplRef(const DefaultSPStorage& sp)
    	{ return sp.pointee_; }

    	friend inline StoredType& GetImplRef(DefaultSPStorage& sp)
    	{ return sp.pointee_; }

    protected:
        // Destroys the data stored
        // (Destruction might be taken over by the OwnershipPolicy)
        void Destroy()
        { delete pointee_; }
        
        // Default value to initialize the pointer
        static StoredType Default()
        { return 0; }
    
    private:
        // Data
        StoredType pointee_;
    };

////////////////////////////////////////////////////////////////////////////////
// class template RefCounted
// Implementation of the OwnershipPolicy used by SmartPtr
// Provides a classic external reference counting implementation
////////////////////////////////////////////////////////////////////////////////    
    template <class P>
    class RefCounted
    {
    public:
        RefCounted() 
        {
            pCount_ = static_cast<unsigned int*>(
                SmallObject<>::operator new(sizeof(unsigned int)));
         
            assert(pCount_);
            *pCount_ = 1;
        }
        
        RefCounted(const RefCounted& rhs) 
        : pCount_(rhs.pCount_)
        {}
        
        // MWCW lacks template friends, hence the following kludge
        template <typename P1>
        RefCounted(const RefCounted<P1>& rhs) 
        : pCount_(reinterpret_cast<const RefCounted&>(rhs).pCount_)
        {}
        
        P Clone(const P& val)
        {
            ++*pCount_;
            return val;
        }
        
        bool Release(const P&)
        {
            if (!--*pCount_)
            {
                SmallObject<>::operator delete(pCount_, sizeof(unsigned int));               
                return true;
            }
            return false;
        }
        
        void Swap(RefCounted& rhs)
        { std::swap(pCount_, rhs.pCount_); }
    
        enum { destructiveCopy = false };

    private:
        // Data
        unsigned int* pCount_;
    };    






    // p189, declaration
    template
    <
	typename T,
	template <class> class OwnershipPolicy = RefCounted,
	// class ConversionPolicy = DisallowConversion,
	// template <class> class CheckingPolicy = AssertCheck,
	template <class> class StoragePolicy = DefaultSPStorage
    >
    class SmartPtr;
    
    // definition, copy from <SmartPtr.h>
    template
    <
        typename T,
        template <class> class OwnershipPolicy,
        // class ConversionPolicy,
        // template <class> class CheckingPolicy,
        template <class> class StoragePolicy
    >
    class SmartPtr
        : public StoragePolicy<T>
        , public OwnershipPolicy<typename StoragePolicy<T>::PointerType>
        // , public CheckingPolicy<typename StoragePolicy<T>::StoredType>
        // , public ConversionPolicy
    {
        typedef StoragePolicy<T> SP;
        typedef OwnershipPolicy<typename StoragePolicy<T>::PointerType> OP;
        // typedef CheckingPolicy<typename StoragePolicy<T>::StoredType> KP;
        // typedef ConversionPolicy CP;
        
    public:
        typedef typename SP::PointerType PointerType;
        typedef typename SP::StoredType StoredType;
        typedef typename SP::ReferenceType ReferenceType;
/*        
        typedef typename Select<OP::destructiveCopy, 
                                SmartPtr, 
                                const SmartPtr>::Result
            CopyArg;
*/    
/*
        SmartPtr()
        { KP::OnDefault(GetImpl(*this)); }
*/    	
        SmartPtr(const StoredType& p) : SP(p)
        { /* KP::OnInit(GetImpl(*this)); */ }
/*    	
    	SmartPtr(CopyArg& rhs)
        : SP(rhs), OP(rhs), KP(rhs), CP(rhs)
        { GetImplRef(*this) = OP::Clone(GetImplRef(rhs)); }

        template
        <
            typename T1,
            template <class> class OP1,
            class CP1,
            template <class> class KP1,
            template <class> class SP1
        >
    	SmartPtr(const SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs)
    	: SP(rhs), OP(rhs), KP(rhs), CP(rhs)
    	{ GetImplRef(*this) = OP::Clone(GetImplRef(rhs)); }

        template
        <
            typename T1,
            template <class> class OP1,
            class CP1,
            template <class> class KP1,
            template <class> class SP1
        >
    	SmartPtr(SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs)
    	: SP(rhs), OP(rhs), KP(rhs), CP(rhs)
    	{ GetImplRef(*this) = OP::Clone(GetImplRef(rhs)); }

        SmartPtr(ByRef<SmartPtr> rhs)
    	: SP(rhs), OP(rhs), KP(rhs), CP(rhs)
        {}
        
        operator ByRef<SmartPtr>()
        { return ByRef<SmartPtr>(*this); }

    	SmartPtr& operator=(CopyArg& rhs)
    	{
    	    SmartPtr temp(rhs);
    	    temp.Swap(*this);
    	    return *this;
    	}

        template
        <
            typename T1,
            template <class> class OP1,
            class CP1,
            template <class> class KP1,
            template <class> class SP1
        >
    	SmartPtr& operator=(const SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs)
    	{
    	    SmartPtr temp(rhs);
    	    temp.Swap(*this);
    	    return *this;
    	}
    	
        template
        <
            typename T1,
            template <class> class OP1,
            class CP1,
            template <class> class KP1,
            template <class> class SP1
        >
    	SmartPtr& operator=(SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs)
    	{
    	    SmartPtr temp(rhs);
    	    temp.Swap(*this);
    	    return *this;
    	}
    	
    	void Swap(SmartPtr& rhs)
    	{
    	    OP::Swap(rhs);
    	    CP::Swap(rhs);
    	    KP::Swap(rhs);
    	    SP::Swap(rhs);
    	}
*/    	
    	~SmartPtr()
    	{
    	    if (OP::Release(GetImpl(*this)))
    	    {
    	        SP::Destroy();
    	    }
    	}
/*    	
    	friend inline void Release(SmartPtr& sp, typename SP::StoredType& p)
    	{
    	    p = GetImplRef(sp);
    	    GetImplRef(sp) = SP::Default();
    	}
    	
    	friend inline void Reset(SmartPtr& sp, typename SP::StoredType p)
    	{ SmartPtr(p).Swap(sp); }
*/
        PointerType operator->()
        {
            // KP::OnDereference(GetImplRef(*this));
            return SP::operator->();
        }

        PointerType operator->() const
        {
            // KP::OnDereference(GetImplRef(*this));
            return SP::operator->();
        }

        ReferenceType operator*()
        {
            // KP::OnDereference(GetImplRef(*this));
            return SP::operator*();
        }
    	
        ReferenceType operator*() const
        {
            // KP::OnDereference(GetImplRef(*this));
            return SP::operator*();
        }
/*    	
        bool operator!() const // Enables "if (!sp) ..."
        { return GetImpl(*this) == 0; }
        
        inline friend bool operator==(const SmartPtr& lhs,
            const T* rhs)
        { return GetImpl(lhs) == rhs; }
        
        inline friend bool operator==(const T* lhs,
            const SmartPtr& rhs)
        { return rhs == lhs; }
        
        inline friend bool operator!=(const SmartPtr& lhs,
            const T* rhs)
        { return !(lhs == rhs); }
        
        inline friend bool operator!=(const T* lhs,
            const SmartPtr& rhs)
        { return rhs != lhs; }

        // Ambiguity buster
        template
        <
            typename T1,
            template <class> class OP1,
            class CP1,
            template <class> class KP1,
            template <class> class SP1
        >
        bool operator==(const SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs) const
        { return *this == GetImpl(rhs); }

        // Ambiguity buster
        template
        <
            typename T1,
            template <class> class OP1,
            class CP1,
            template <class> class KP1,
            template <class> class SP1
        >
        bool operator!=(const SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs) const
        { return !(*this == rhs); }

        // Ambiguity buster
        template
        <
            typename T1,
            template <class> class OP1,
            class CP1,
            template <class> class KP1,
            template <class> class SP1
        >
        bool operator<(const SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs) const
        { return *this < GetImpl(rhs); }

    private:
        // Helper for enabling 'if (sp)'
        class Tester
        {
            void operator delete(void*);
        };
        
    public:
        // enable 'if (sp)'
        operator Tester*() const
        {
            if (!*this) return 0;
            static Tester t;
            return &t;
        }

    private:
        // Helper for disallowing automatic conversion
        struct Insipid
        {
            Insipid(PointerType) {}
        };
        
        typedef Select<CP::allow, PointerType, Insipid>::Result
            AutomaticConversionResult;
    
    public:        
        operator AutomaticConversionResult() const
        { return GetImpl(*this); }
*/
    };    



    // p158, a test version
    // 一個簡單的模擬。並不存在於原本的 Loki SmallPtr.h 中。
    template <class T>
    class SmartPtrTest
    {
    public:
	explicit SmartPtrTest(T* pointee) : pointee_(pointee) { } 
	SmartPtrTest& operator=(const SmartPtrTest& other);
	~SmartPtrTest() {  delete pointee_; }
	T& operator*() const  {
		//...
		return *pointee_;
	}
	T* operator->() const  {
		//...
		return pointee_;
	}
	
	void Get() { }
	void Set() { }
	void Release() { std::cout << "SmartPtr::Release" << std::endl; } 
			
    private:
	T* pointee_;
	//...
    };
    
} // namespace Loki

#endif // SMARTPTR_INC_
