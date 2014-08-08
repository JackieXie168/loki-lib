// 《C++設計新思維》（Modern C++ Design）第05章 Functor 測試程式
// last update : 20030402
// 適用編譯器：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1) 
//             GNU-C++ 2.95.3-5 (Cygwin special,20010316) 

// g291[x] 大部份都 OK，但開發到 Functor::Functor(std::auto_ptr<Impl>) 時掛了。  
// cb6[x] partial specialization is not so GOOD in CB6

#include "TypeList-jj.h"
#include "Functor-jj.h"
#include <iostream>
#include <string>
using namespace std;
using namespace Loki;
using namespace Loki::TL;


// p112
struct TestFunctor {
    void operator()(int i, double d) {
    	 cout << "TestFunctor::operator()(" << i << "," << d 
    	      << ") called." << endl;
    }
};

// p113
void TestFunction(int i, double d) {
     cout << "TestFunction(" << i << "," << d 
          << ") called." << endl;
}

// p114
const char* TestFunction2(double, double)
{ 
    static const char buffer[] = "Hello World";
    return buffer;
}    	

// jjhou add.
template <typename T1, typename T2>
struct TestTemp2Functor {
    void operator()(T1 p1, T2 p2) {
    	 cout << "TestTemp2Functor::operator()(" << p1 << "," << p2 
    	      << ") called." << endl;
    }
};

// jjhou add.
template <typename T1>
struct TestTemp1Functor {
    void operator()(T1 p1) {
    	 cout << "TestTemp1Functor::operator()(" << p1 
    	      << ") called." << endl;
    }
};

// jjhou add.
template <typename R, typename T1, typename T2>
struct TestTemp3Functor {
    R operator()(const T1& p1, const T2& p2) {
    	 cout << "TestTemp3Functor::operator()(" << p1 << "," << p2 
    	      << ") called." << endl;
    	 return R();     
    }
};

// p119
class Parrot {
public:
	void Eat() const {  // 不論是否為 const，都不影響成為 Functor 的第二引數。
		cout << "Tsk, knick, tsk...\n";
	}
	void Speak() const {// 不論是否為 const，都不影響成為 Functor 的第二引數。
		cout << "Oh Captain, my Captain!\n";
	}
};


//------------------------------------------------------------
int main()
{
    // test TypeList	
    typedef TYPELIST_2(signed char, int) TypeList2;
    cout << sizeof(TypeAt<TypeList2,0>::Result) << endl;  // 1
    cout << sizeof(TypeAt<TypeList2,1>::Result) << endl;  // 4  
    cout << sizeof(TypeAtNonStrict<TypeList2,0>::Result) << endl;  // 1          
    cout << sizeof(TypeAtNonStrict<TypeList2,1>::Result) << endl;  // 4    
    cout << sizeof(TypeAtNonStrict<TypeList2,2>::Result) << endl;  // 1  (NullType) 
    cout << sizeof(TypeAtNonStrict<TypeList2,9>::Result) << endl;  // 1  (NullType)
              
    {                 
    // p112. test Generalized Functor for functor       
    TestFunctor f;
    Functor<void, TYPELIST_2(int, double)> cmd(f);
    // 注意，不能寫如下，我想是編譯器的臭蟲 
    // Functor<void, TYPELIST_2(int, double)> cmd(TestFunctor());    
    
    f(4, 4.5);		// TestFunctor::operator()(4,4.5) called.
    cmd(4, 4.5); 	// TestFunctor::operator()(4,4.5) called.
    /* 除錯訊息，有價值，保留。
    match for call to 
       (Loki::Functor<void, 
                      Loki::TL::Typelist<int, 
                                         Loki::TL::Typelist<double, 
                                                            Loki::NullType> > >) 
                     (int, double)
    */
    }  
  
    { 
    // test Generalized Functor for functor      	
    TestTemp2Functor<int, char> f;
    Functor<void, TYPELIST_2(int, char)> cmd(f);
    // 注意，不能寫如下，我想是編譯器的臭蟲 
    // Functor<void, TYPELIST_2(int, char)> cmd(TestTempFunctor<int, char>());
    
    f(2, 'j');		// TestTemp2Functor::operator()(2,j) called.
    cmd(2, 'j'); 	// TestTemp2Functor::operator()(2,j) called.
    }

    {
    // test Generalized Functor for functor     	
    TestTemp1Functor<string> f;
    Functor<void, TYPELIST_1(string)> cmd(f);
    
    f(string("jjhou"));		// TestTemp1Functor::operator()(jjhou) called.
    cmd(string("jjhou")); 	// TestTemp1Functor::operator()(jjhou) called.
    }

    {
    // p113. test Generalized Functor for function pointer  
    Functor<void, TYPELIST_2(int, double)> cmd(&TestFunction);   // 不能沒有 '&'
    
    TestFunction(2, 3.8);	// TestFunction(2,3.8) called.
    cmd(2, 3.8);		// TestFunction(2,3.8) called.
    }

    {
    // p114. test type conversion for Generalized Functor  
    Functor<string, TYPELIST_2(int, int)> cmd(&TestFunction2); // 不能沒有 '&'
    cout << cmd(10,10).substr(6) << endl;    // World
    }

    {
    // p119. test Generalized Functor for member functions
    Parrot geronimo;
    // Define two Commands
    Functor<void,NullType> cmd1(&geronimo, &Parrot::Eat),
	                   cmd2(&geronimo, &Parrot::Speak);
    // Invoke each of them 
    cmd1();		// Tsk, knick, tsk...
    cmd2();  		// Oh Captain, my Captain!
    
    geronimo.Eat();	// Tsk, knick, tsk...
    geronimo.Speak();	// Oh Captain, my Captain!
    }    

    {
    // p119, test binder
    TestTemp2Functor<int, int> f; 
    Functor<void, TYPELIST_2(int, int)> cmd1(f);
    Functor<void, TYPELIST_1(int)> cmd2(BindFirst(cmd1,10));
    cmd2(99);  // TestTemp2Functor::operator()(10,99) called.
    /*
    注意，編譯器會喚起以下 ctor（這份信息是原本編譯錯誤時給出的錯誤訊息）：
    `Loki::Functor<void, 
                   Loki::TL::Typelist<int, 
                                      Loki::TL::Typelist<int, 
                                                         Loki::NullType> > >
       ::Functor(Loki::Functor<void, 
                               Loki::TL::Typelist<int, 
                                                  Loki::TL::Typelist<int, 
                                                        Loki::NullType> > > const&)'
    */
    }

    {
    /* 以下有警告，且不正常結束。尚未找出原因	
    // p121, test type conversion + binding
    TestTemp3Functor<const char*, int, int> f;
    Functor<const char*, TYPELIST_2(char, int)> cmd1(f);
    cmd1(9,9);  // TestTemp3Functor::operator()(9,9) called.
    Functor<string, TYPELIST_1(int)> cmd2(BindFirst(cmd1,10));
    // 上一行可以。但如將 int 改為 double，像 p121 那樣，就會有 warning
    cmd2(15);      // TestTemp3Functor::operator()(10,15) called.
                   // abnormal program termination (jjhou: I don't know why)
    */
    }

    {
    // p122, test chaining	
    Functor<string, TYPELIST_2(int, int)> cmd1(&TestFunction2); // 不能沒有 '&'
    Functor<string, TYPELIST_2(int, int)> cmd2(&TestFunction2); // 不能沒有 '&'
//! Functor<string, TYPELIST_2(int, int)> cmd3(Chain(cmd1, cmd2));
/* 引發以下錯誤
In function Fun2 Loki::Chain(const Fun1&, const Fun2&) 
[with
   Fun1 = Loki::Functor<std::string, 
                        Loki::TL::Typelist<int,
                                           Loki::TL::Typelist<int, 
                                                              Loki::NullType> > >, 
   Fun2 = Loki::Functor<void,
                        Loki::TL::Typelist<int, 
                                           Loki::TL::Typelist<double, 
                                                              Loki::NullType> > >
]:

../ch5test.cpp:540: cannot allocate an object of type `
Loki::Chainer<Loki::Functor<std::string, 
                            Loki::TL::Typelist<int,
                                               Loki::TL::Typelist<int, 
                                                                  Loki::NullType> > >, 
              Loki::Functor<void, 
                            Loki::TL::Typelist<int, 
                                               Loki::TL::Typelist<double, 
                                                                  Loki::NullType> > > 
             >
../ch5test.cpp:540:   because the following virtual functions are abstract:
../ch5test.cpp:304:     
R Loki::FunctorImpl<R, 
                    Loki::TL::Typelist<P1,
                                       Loki::TL::Typelist<P2, 
                                                          Loki::NullType> >,
                                       Loki::SingleThreaded>
   ::operator()(P1, P2) 
[with R = void, P1 = int, P2 = double]
*/    	
    }	
}
