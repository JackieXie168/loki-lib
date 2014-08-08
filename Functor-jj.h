// Loki 侯捷調整版
// 檔名 : Functor-jj.h
// 根據 : Loki's Functor.h
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

#ifndef FUNCTOR_INC_
#define FUNCTOR_INC_

#include "TypeList-jj.h"
#include "NullType-jj.h"
#include "Threads-jj.h"
#include <memory>    // jjhou add. for auto_ptr

namespace Loki
{
    using namespace TL;
    
    // p107
    // generic form.
    template <typename R, class TList,
              template <class> class ThreadingModel = DEFAULT_THREADING>
    class FunctorImpl;
    
    // partial specialization, 0 param. version
    template <typename R>
    class FunctorImpl<R, NullType>
    {
    public:
            virtual R operator()() = 0;
            virtual FunctorImpl* Clone() const = 0;
            virtual ~FunctorImpl() {}
    };
    
    // partial specialization, 1 param. version    
    template <typename R, typename P1>
    class FunctorImpl<R, TYPELIST_1(P1)>
    {
    public:
            virtual R operator()(P1) = 0;
            virtual FunctorImpl* Clone() const = 0;
            virtual ~FunctorImpl() {}
    };
    
    // partial specialization, 2 param. version      
    template <typename R, typename P1, typename P2>
    class FunctorImpl<R, TYPELIST_2(P1, P2)>
    {       // 再次提醒，討論範圍只限兩個參數
    public:
            virtual R operator()(P1, P2) = 0;
            virtual FunctorImpl* Clone() const = 0;
            virtual ~FunctorImpl() {}
    };
    
    
    
    // p110
    template <class ParentFunctor,  typename Fun>
    class FunctorHandler : public FunctorImpl <
                                    typename ParentFunctor::ResultType,
                                    typename ParentFunctor::ParmList >
    {
	typedef typename ParentFunctor::ResultType ResultType;
	
    public:
	FunctorHandler(const Fun& fun) : fun_(fun) {}

	FunctorHandler* Clone() const
	{ return new FunctorHandler(*this); }

	ResultType operator()() {
		return fun_();
	}
 	ResultType operator()(typename ParentFunctor::Parm1 p1){
		return fun_(p1);
	}	
	ResultType operator()(typename ParentFunctor::Parm1 p1,
		              typename ParentFunctor::Parm2 p2) {
		return fun_(p1,p2);
	}
    private:
        Fun fun_;
    };
    
    
    // p117
    template <class ParentFunctor, 
              typename PointerToObj,
	      typename PointerToMemFn>
    class MemFunHandler : public FunctorImpl <
			             typename ParentFunctor::ResultType,
			             typename ParentFunctor::ParmList >
    {
	typedef typename ParentFunctor::ResultType ResultType;

    public:	
        // 為什麼第二參數不是 pass by const reference?
	MemFunHandler(const PointerToObj& pObj, PointerToMemFn pMemFn)
	  : pObj_(pObj), pMemFn_(pMemFn) {}

	MemFunHandler* Clone() const
	{ return new MemFunHandler(*this); }

 	ResultType operator()()  {
		return ((*pObj_).*pMemFn_)();
	}
	ResultType operator()(typename ParentFunctor::Parm1 p1) 	{
		return ((*pObj_).*pMemFn_)(p1);
	}
	ResultType operator()(typename ParentFunctor::Parm1 p1, 
		typename ParentFunctor::Parm2 p2)  {
		return ((*pObj_).*pMemFn_)(p1, p2);
	}

    private:
	PointerToObj pObj_;
	PointerToMemFn pMemFn_;
    };
    
    
    
    // p108
    // Functor with any number and types of arguments
    // jjhou: 我把第二參數設計為帶有 default argument NullType，對 user 比較更方便
    template <typename R, class TList = NullType>
    class Functor
    {
    typedef FunctorImpl<R, TList> Impl;	
    // 上行原在 private 區，被我移至此，否則以下的 "explicit 1-arg ctor" 編譯錯誤。
    
    public:    
            // 以下四式宣告於 p108，定義來自 <Functor.h> 
            Functor() : spImpl_(0)  {}
            
            Functor(const Functor& rhs) 
              : spImpl_( rhs.spImpl_.get()->Clone() ) { }  // 這和 <Functor.h> 有點不同
          //! : spImpl_(Impl::Clone(rhs.spImpl_.get())) {} // 這是 <Functor.h> 原內容
              
            explicit Functor(std::auto_ptr<Impl> spImpl) 
              : spImpl_(spImpl) {}
                // g291 不通過上行，因為其 auto_ptr<> 實作不良   
        
            Functor& operator=(const Functor& rhs)
            {
                Functor copy(rhs);
                // swap auto_ptrs by hand
                Impl* p = spImpl_.release();
                spImpl_.reset(copy.spImpl_.release());
                copy.spImpl_.reset(p);
                return *this;
            }
    
            // p109
            typedef R 		ResultType;
            typedef TList 	ParmList;
            typedef typename TypeAtNonStrict<TList,0,EmptyType>::Result  Parm1;
            typedef typename TypeAtNonStrict<TList,1,EmptyType>::Result  Parm2;
            
            // p109
            R operator()()                   { return (*spImpl_)();      }
            R operator()(Parm1 p1)           { return (*spImpl_)(p1);    }
            R operator()(Parm1 p1,Parm2 p2)  { return (*spImpl_)(p1,p2); } 
               
            // p110, (1) ctor for functor and function pointer        
            template <class Fun>    
            Functor(const Fun& fun) 
              : spImpl_(new FunctorHandler<Functor, Fun>(fun))  // p111
            { 
            	/* 以下只為測試。測試 OK。
            	cout << "Functor::Functor" << endl; 
            	cout << Length<TList>::value << endl;  
            	cout << sizeof(TypeAt<TList,0>::Result) << endl;  
            	cout << sizeof(TypeAt<TList,1>::Result) << endl;  
            	
            	// 以下三式效果相同      	
            	spImpl_->operator()(4, 4.5);
            	(*spImpl_).operator()(4, 4.5);
            	(*spImpl_)(4, 4.5);
            	*/
            }
            
            // 書上沒有, (2) ctor for member function. 可參考 <Functor.h>  
            // 為什麼第二參數不是 pass by const reference?      
            template <class PointerToObj, class PointerToMemFn>    
            Functor(const PointerToObj& pObj, PointerToMemFn pMemFn) 
              : spImpl_(new MemFunHandler<Functor, PointerToObj, PointerToMemFn>(pObj, pMemFn))  
            { }         
                   
            
            // (3) ctor for BinderFirst
            // 注意，new for BinderFirst object 不在這兒進行，而在 BindFirst()  
            
    private:
            // p108
            // Handy type definition for the body type
            std::auto_ptr<Impl> spImpl_;
            // g291 不通過上行，因為其 auto_ptr<> 實作不良
    };
    
    
    // p120
    template <class Incoming>
    class BinderFirst 
	: public FunctorImpl<typename Incoming::ResultType, 
		             typename Incoming::ParmList::Tail>
    {
	typedef Functor<typename Incoming::ResultType, 
	                typename Incoming::ParmList::Tail> Outgoing;
	typedef typename Incoming::Parm1 	Bound;
	typedef typename Incoming::ResultType 	ResultType;
    
    public:
	BinderFirst(const Incoming& fun, Bound bound)
	  : fun_(fun), bound_(bound) 	{ }

	BinderFirst* Clone() const
	{ return new BinderFirst(*this); }


	ResultType operator()() {
		return fun_(bound_);
	}
	ResultType operator()(typename Outgoing::Parm1 p1) {
		return fun_(bound_, p1);
	}
	ResultType operator()(typename Outgoing::Parm1 p1,
		              typename Outgoing::Parm2 p2) {
		return fun_(bound_, p1, p2);
	}
	
    private:
	Incoming fun_;
	Bound bound_;
    };    
    
    
    // p121, Aux. Function
    template <class R, class TList>
    Functor<R, typename TList::Tail>
    BindFirst(const Functor<R,TList>& fun, typename TList::Head bound)
    {
	typedef Functor<R,TList> 		Incoming;
	typedef Functor<R,typename TList::Tail> Outgoing;

	return Outgoing(std::auto_ptr<typename Outgoing::Impl>(
		            new BinderFirst<Incoming>(fun, bound)));
    }
    
    
    
    // p122 
    template <class Fun1, class Fun2>
    class Chainer : public Fun2::Impl  // 為什麼不（能）用 typename Fun2::Impl ?
    {
        typedef Fun2 Base;

    public:
        
/*
        // 為什麼以下三行編譯不過？  
        using typename Base::ResultType;  
        using typename Base::Parm1;
        using typename Base::Parm2;
*/        
        Chainer(const Fun1& fun1, const Fun2& fun2) 
          : f1_(fun1), f2_(fun2) {}
       
	Chainer* Clone() const
	{ return new Chainer(*this); }        
/* 
        // operator() implementations for up to 15 arguments
        // 看來好像不能把參數個數不同的 functors 串鏈起來。
        ResultType operator()()
        { return f1_(), f2_(); }

        ResultType operator()(Parm1 p1)
        { return f1_(p1), f2_(p1); }
        
        ResultType operator()(Parm1 p1, Parm2 p2)
        { return f1_(p1, p2), f2_(p1, p2); }
*/               
    private:
        Fun1 f1_;
        Fun2 f2_;
    };
    

    // Chains two functor calls one after another
    template <class Fun1, class Fun2>
    Fun2 Chain(
        const Fun1& fun1,
        const Fun2& fun2)
    {
        return Fun2(std::auto_ptr<typename Fun2::Impl>(
                       new Chainer<Fun1, Fun2>(fun1, fun2)));
    }    
   
    
} // namespace Loki


#endif // FUNCTOR_INC_
