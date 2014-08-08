// 《C++設計新思維》（Modern C++ Design）第02章 Techniques 測試程式
// last update : 20030402
// 適用編譯器：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1) 
//             GNU-C++ 2.95.3-5 (Cygwin special,20010316) 

#include "TypeManip-jj.h"  
#include "TypeTraits-jj.h"  

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <complex>
using namespace std;
using namespace Loki;

//--------------------------------------------------------------
class Int
{
public:
  Int(int i=0) { _i = i; }
private:
  int _i;
};

struct Widget
{
  Widget(int, int) { cout << "Widght(int,int)" << endl;  }
  Widget(Int, int) { cout << "Widght(Int,int)" << endl;  }
  Widget()         { cout << "Widght()"        << endl;  }
};

// p31
template <class T, class U>
T* Create(const U& arg)
{
    return new T(arg);
}

// 加上以下三個 explicit template arguments 版本。
// 型式(1)：T 指定為 Widget，U 被推導為 char
template <>
Widget* Create<Widget>(const char& arg) {
    cout << "form1" << endl;
    return new Widget(arg, -1);
}
// 型式(2)：T 指定為 Widget，U 被推導為 int
template <>
Widget* Create<Widget>(const int& arg) {
    cout << "form2" << endl;
    return new Widget(arg, -1);
}
// 型式(3)：T 指定為 Widget，U 被推導為 Int
template <>
Widget* Create<Widget>(const Int& arg) {
    cout << "form3" << endl;
    return new Widget(arg, -1);
}

/*
// p31, Illegal code -- Invalid template declaration
// 由於 function template 無「偏特化機制」，使我們無法寫出下面這樣的程式碼
//   （為了讓 Create() "只要一遇到 T 為 Widget，不管第二個 template param 
//     是什麼，都以 -1 做為 Widget ctor 的第二引數"）
template <class U>
Widget* Create<Widget, U>(const U& arg) {
    return new Widget(arg, -1);
}
*/

// p32
// 為了解決上述問題，可以這麼做。
// 但這使得呼叫端得先準備一個 T dummy object 用來喚起
// 以下第一式，或一個 Widget dummy object 用來喚起以下第二式。
template <class T, class U>
T* Create(const U& arg, T /* dummy */)
{
    cout << "form4" << endl;
    return new T(arg);
}

template <class U>
Widget* Create(const U& arg, Widget /* dummy */)
{
    cout << "form5" << endl;
    return new Widget(arg, -1);
}


// p32。以下使用 Type2Type，改善上述兩個 overloaded function templates
template <class T, class U>
T* Create(const U& arg, Type2Type<T>)
// 侯捷：上式如果如下，是否效果相同（甚至更富彈性，且更對 client 方便）：
//    T* Create(const U& arg, T)
// 噢不，我們就是為了減輕 "client 產生非必要之 temp object" 所以才進行至此的呀。 
{
    cout << "form6" << endl;
    return new T(arg);
}
template <class U>
Widget* Create(const U& arg, Type2Type<Widget>)
{
    cout << "form7" << endl;
    return new Widget(arg, -1);
}
//--------------------------------------------------------------

class W  // a polymorphical class
{
public:
  ~W() { cout << "W::dtor" << endl; }
private:
  string str[3];     
};

template <typename T, bool isPolymorphic>
class NiftyContainer
{
public:

    typedef typename Loki::Select<isPolymorphic, T*, T>::Result  ValueType;
    /*
    上行原本沒有 typename，那麼 g291[o], g295[o], but g32[w]: 
    warning: `typename Loki::Select<isPolymorphic, T*,T>::Result' is implicitly a typename
    warning: implicit typename is deprecated, please see the documentation for details
    */    
    
    size_t elemSize() { return sizeof(ValueType); }
private:
    vector<ValueType> v;
};    

//--------------------------------------------------------------
// cb6[o] cb5[x] g295[x] 291[x]
class Base { };
class Derived : public Base { };

void funcD(Derived) { };
void funcB(Base)    { };

// 以下內容模擬 Loki "TypeManip.h"
template <class T, class U>
class Test
{
public:
    enum { exists = 
                sizeof(Loki::Conversion<T,U>::Test(Loki::Conversion<T,U>::MakeT())) 
             == sizeof(Loki::Private::Small) 
         };
    enum { sameType = false };     
};

#define JJSUPERSUBCLASS(T, U) \
    (Test<const U*, const T*>::exists && \
    !Test<const T*, const void*>::sameType)

//--------------------------------------------------------------

class Test2
{
public:
    void Hello() { cout << "hello in member function" << endl; }
};

void Hello() { cout << "hello in global function" << endl; }

//--------------------------------------------------------------




int main()
{
  {	
  // 在 client 端，以下缺點是：呼叫 function template 必須使用 explicit template argument.	 
  Widget* w1 = Create<Widget>(5);        // form2 widget(int,int)
  Widget* w2 = Create<Widget>('x');      // form1 widget(int,int)
  Widget* w3 = Create<Widget>(Int(5));   // form3 widget(Int,int)

  // 在 client 端，以下缺點是：需做出一些沒必要的 temp object（可能是複雜類別）
  Widget* w4 = Create(5, Widget());      // form5 widget(int,int)
  Widget* w5 = Create(Int(5), Widget()); // form5 widget(Int,int)
  Int*    v1 = Create(7, Int());         // form4
  int*    v2 = Create(7, int());         // form4
  char*   v3 = Create(7, char());        // form4
  string* v4 = Create("x", string());    // form4

  // 在 client 端，以下是理想型式。做出之 temp object 都是 simple objects.
  string* pStr = Create("Hello", Type2Type<string>());  // form6
  Widget* pW   = Create(100, Type2Type<Widget>());      // form7 widget(int,int)
  }

  //-----------------------------------------------------------------------------

  {
   cout << sizeof(complex<long>) << endl;   // 8
   cout << sizeof(W) << endl;		    // 72(cb6), 12(g291,g295)
   
   NiftyContainer<complex<long>, false> vc; 
   cout << vc.elemSize() << endl;           // 8, good
   
   NiftyContainer<W, true> vw;
   cout << vw.elemSize() << endl;           // 4, good
  }
  
  //-----------------------------------------------------------------------------

    //! funcD(Base());    // ERROR. Cannot convert 'Base' to 'Derived'
    funcB(Derived());     // OK. implicit conversion
    // 以上首先測試函式呼叫時，引數的自動轉型。

    cout << sizeof(Loki::Private::Small) << endl;             // 1, good
    cout << sizeof(Loki::Private::Big) << endl;               // 2, good
/*  
    Loki::Conversion<double,int> conv1;              // 需知：double 可轉型為 int
    conv1.Test(conv1.MakeT());                       // 喚起：MakeT(), Test(U)
  
    // 以下，只要使用 sizeof()，真的就沒有喚起 evaluated function 耶
    cout << sizeof(conv1.Test(conv1.MakeT())) << endl;  // 1, 不再輸出 Make(T), Test(U) 等訊息 
    cout << (sizeof(conv1.Test(conv1.MakeT())) ==
             sizeof(Loki::Private::Small)) << endl;           // 1, good.
    // 上述兩個輸出都是 1，所以 enumerator "exists" 理當為 true。
    // 為什麼稍後的測試卻為 false? 不得已我只好為 Conversion 添加一個 Exists()，
    // 輸出各項結果。結果顯示一切正常，卻為什麼稍後的 exists 不正確呢？
    cout << Loki::Conversion<double, int>::Exists() << endl;  // 111 1

    // 以下是把上例的 <double,int> 改為 <int,double>，再測試一次
    Loki::Conversion<int,double> conv2;
    conv2.Test(conv2.MakeT());                          // MakeT(), Test(U)
    cout << sizeof(conv2.Test(conv2.MakeT())) << endl;  // 1

    // 以下測試 Base, Derived 的轉型關係。結果正確。
    // 卻為什麼稍後的 exists 不正確呢？
    Loki::Conversion<Derived,Base> conv3;
    conv3.Test(conv3.MakeT());                            // MakeT(), Test(U)
    cout << Loki::Conversion<Derived, Base>::Exists() << endl;  // 111 1

    Loki::Conversion<Base,Derived> conv4;
    conv4.Test(conv4.MakeT());                            // MakeT(), Test(...)
    cout << Loki::Conversion<Base, Derived>::Exists() << endl;  // 210 0


    // 我發現，enumerator exists 似乎總是為 0。TypeManip.h 何處出錯？
    cout << Loki::Conversion<Derived, Base>::exists << endl;  // 0 why???
    cout << Loki::Conversion<Base, Derived>::exists << endl;  // 0

    cout << Loki::Conversion<double, int>::exists << endl;    // 0 why???
    cout << Loki::Conversion<int, double>::exists << endl;    // 0 why???
    cout << Loki::Conversion<char, char*>::exists << endl;    // 0
    cout << Loki::Conversion<char*, char>::exists << endl;    // 0
//  cout << Loki::Conversion<size_t, vector<int> >::exists << endl;

    cout << SUPERSUBCLASS(Base,Derived) << endl;        // 0 why???
    cout << SUPERSUBCLASS(Derived,Base) << endl;        // 0
    cout << SUPERSUBCLASS_STRICT(Base,Derived) << endl; // 0 why???
    cout << SUPERSUBCLASS_STRICT(Derived,Base) << endl; // 0
    
    // 為此，我在本程式直接檢驗 enum 的語法。我只是把 "TypeManip.h" 中的
    //   Loki::Conversion::exists（以及 sameType）搬到本檔的 Test 內，
    //   並將 SUPERSUBCLASS 搬到本檔的 JJSUPERSUBCLASS 內。
    // 執行結果良好，正確。
    // 這麼一來我就不知道為什麼前面會出錯了 :(
    cout << endl;
    cout << Test<Derived, Base>::exists << endl;  // 1
    cout << Test<Base, Derived>::exists << endl;  // 0

    cout << Test<double, int>::exists << endl;    // 1
    cout << Test<int, double>::exists << endl;    // 1
    cout << Test<char, char*>::exists << endl;    // 0
    cout << Test<char*, char>::exists << endl;    // 0    
    
    cout << JJSUPERSUBCLASS(Base,Derived) << endl;        // 1
    cout << JJSUPERSUBCLASS(Derived,Base) << endl;        // 0
    
    // 我在 ch2-36-2.cpp 中將 "TypeManip.h" 的相關部分直接搬到檔案中，答案還是錯。
    
  //-----------------------------------------------------------------------------
*/
  
  // cb5[x], cb6[x], 
  // g291[o]but result is Error!
  // g295[o],g32[o] 執行結果勉強可看
  
    {
    const bool iterIsPtr = TypeTraits<vector<int>::iterator>::isPointer;
    cout << iterIsPtr << endl;	// 0,why?
    cout << "vector<int>::iterator is " << (iterIsPtr ? "fast" : "smart") << '\n';
    }

    {
    const bool iterIsPtr = TypeTraits<list<int>::iterator>::isPointer;
    cout << iterIsPtr << endl;	// 0,good
    cout << "list<int>::iterator is " << (iterIsPtr ? "fast" : "smart") << '\n';
    }

    {
    const bool iterIsPtr = TypeTraits<int*>::isPointer;
    cout << iterIsPtr << endl;	// 1,good
    cout << "list<int>::iterator is " << (iterIsPtr ? "fast" : "smart") << '\n';
    }

    {
    const bool IsRef = TypeTraits<int>::isReference;
    cout << IsRef << endl;    // 0
    }

    {
    // statement below g32[w]:     	
    const bool IsRef = TypeTraits<int&>::isReference;
    cout << IsRef << endl;    // 1
    }

    {
    // statement below g32[w]:     	
    const bool IsRef = TypeTraits<Test2&>::isReference;
    cout << IsRef << endl;    // 1
    }

    {
    typedef void (Test2::*PMFofTest2)();
    typedef void (*PF)();    
    cout << TypeTraits<PMFofTest2>::isMemberPointer;  	// 0,why???
    cout << TypeTraits<PF>::isMemberPointer;  		// 0
    cout << TypeTraits<int*>::isMemberPointer;  	// 0 
  }
  //
}

/* note: warning in G32 :
../TypeTraits-jj.h: In instantiation of `Loki::TypeTraits<int&>::UnVolatile<int&>':
../ch02test.cpp:309:   instantiated from `Loki::TypeTraits<int&>'
../ch02test.cpp:309:   instantiated from here
../TypeTraits-jj.h:218: warning: ignoring `volatile' qualifiers on `int&'
../TypeTraits-jj.h:218: warning: ignoring `volatile' qualifiers on `int&'
../TypeTraits-jj.h:219: warning: ignoring `volatile' qualifiers on `int&'
../TypeTraits-jj.h:220: warning: ignoring `volatile' qualifiers on `int&'
../TypeTraits-jj.h: In instantiation of `Loki::TypeTraits<Test2&>::UnVolatile<Test2&>':
../ch02test.cpp:315:   instantiated from `Loki::TypeTraits<Test2&>'
../ch02test.cpp:315:   instantiated from here
../TypeTraits-jj.h:218: warning: ignoring `volatile' qualifiers on `Test2&'
../TypeTraits-jj.h:218: warning: ignoring `volatile' qualifiers on `Test2&'
../TypeTraits-jj.h:219: warning: ignoring `volatile' qualifiers on `Test2&'
../TypeTraits-jj.h:220: warning: ignoring `volatile' qualifiers on `Test2&'
*/
