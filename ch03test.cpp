// 《C++設計新思維》（Modern C++ Design）第03章 Typelists 測試程式
// last update : 20030402
// 適用編譯器：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1) 
//             GNU-C++ 2.95.3-5 (Cygwin special,20010316) 

// g291[o]（但 GenScatterHierarchy's object 不通過）
// cb6[x] cb5[x]: partial specialization is not so GOOD in CB6 & CB5

#include "TypeList-jj.h"

// test:
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

class Window { };
class Widget : public Window { };
class Button : public Widget { };
class TextField : public Widget { };
class ScrollBar : public Widget { };
class Object { };
class Document : public Object { };
class View : public Object { };

template <class T>
struct Holder
{
    Holder() { cout << "Holder::ctor" << endl; }	
    T value_;
};

template <class T, class Base>
class EventHandler : public Base
{
public:
    virtual void OnEvent(T& obj, int eventId) 
    { cout << "OnEvent:" << eventId << endl;  }
};    

#include <iostream>
#include <string>
using namespace std;
using namespace Loki;
using namespace Loki::TL;

int main()
{
    {
    // p52
    typedef Typelist<signed char,
                Typelist<short int,
                    Typelist<int,
                        Typelist<long int, NullType> > > >
            SignedIntegrals;
    }

    {
    // p53
    typedef TYPELIST_4(signed char, short int, int, long int)
            SignedIntegrals;

    SignedIntegrals::Tail::Tail::Tail li;
    cout << Length<SignedIntegrals>::value << endl;  // 4,good

    // p55
    TypeAt<SignedIntegrals, 0>::Result var0;
    TypeAt<SignedIntegrals, 1>::Result var1;
    TypeAt<SignedIntegrals, 2>::Result var2;
    TypeAt<SignedIntegrals, 3>::Result var3;
    cout << sizeof(var0) << endl;                     // 1, good for "signed char"
    cout << sizeof(var1) << endl;                     // 2, good for "short int"
    cout << sizeof(var2) << endl;                     // 4, good for "int"
    cout << sizeof(var3) << endl;                     // 4, good for "long int"

    // p56
    cout << IndexOf<SignedIntegrals, NullType>::value    << endl; // -1,good
    cout << IndexOf<SignedIntegrals, signed char>::value << endl; //  0,good
    cout << IndexOf<SignedIntegrals, short int>::value   << endl; //  1,good
    cout << IndexOf<SignedIntegrals, int>::value         << endl; //  2,good
    cout << IndexOf<SignedIntegrals, long int>::value    << endl; //  3,good

    // p57
    typedef Append<SignedIntegrals,
                   TYPELIST_3(float, double, long double)>::Result SignedTypes;
    cout << Length<SignedTypes>::value << endl;          // 7,good

    // p58
    typedef Erase<SignedTypes, float>::Result SomeSignedTypes;
    cout << Length<SomeSignedTypes>::value << endl;      // 6,good

    typedef Append<SomeSignedTypes,
                   TYPELIST_3(int, int*, int)>::Result SomeSignedTypes2;
    cout << Length<SomeSignedTypes2>::value << endl;     // 9,good
    // p59
    typedef EraseAll<SomeSignedTypes2, int>::Result SomeSignedTypes3;
    cout << Length<SomeSignedTypes3>::value << endl;     // 6,good
    }

    {
    typedef TYPELIST_4(Widget, Button, TextField, ScrollBar) SomeTypes1;
    cout << Length<NoDuplicates<SomeTypes1>::Result>::value << endl;  // 4,good
    typedef Append<SomeTypes1,
                   TYPELIST_3(Button, Widget, View)>::Result SomeTypes2;
    cout << Length<SomeTypes2>::value << endl;                        
    // 7,good: Widget, Button, TextField, ScrollBar, Button, Widget, View
    
    // p59
    cout << Length<NoDuplicates<SomeTypes2>::Result>::value << endl;  
    // 5,good: Widget, Button, TextField, ScrollBar, View
    cout << Length<SomeTypes2>::value << endl;                  
    // 7 still,good: Widget, Button, TextField, ScrollBar, Button, Widget, View
    
    // p60
    cout << Length<Replace<SomeTypes2,View,Document>::Result>::value << endl;  
    // 7,good: Widget, Button, TextField, ScrollBar, Button, Widget, Document
    
    // p61
    cout << Length< NoDuplicates< ReplaceAll<SomeTypes2,Button,Document>::Result >::Result >::value << endl;  
    // 5,good: Widget, Document, TextField, ScrollBar, View
    }

    {
    typedef GenScatterHierarchy<TYPELIST_3(int,string,Widget), Holder>
            WidgetInfo;
    WidgetInfo obj;	// g291[x], g32[o]
    // Holder::ctor 三次
    string name = (static_cast< Holder<string>& >(obj)).value_;
    int    i    = (static_cast< Holder<int>& >(obj)).value_;
    Widget w    = (static_cast< Holder<Widget>& >(obj)).value_;        
    }

    {
    // p74	
    typedef GenLinearHierarchy<TYPELIST_3(Window,Button,ScrollBar), EventHandler>
            MyEventHandler;
    MyEventHandler obj;
    //! obj.OnEvent(Window(), 1);
    //! obj.OnEvent(Button(), 2);    
    //! obj.OnEvent(ScrollBar(), 3);    
    // 以上這些不通過，是合理的。
    }
}
