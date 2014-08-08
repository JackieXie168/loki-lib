// 《C++設計新思維》（Modern C++ Design）第07章 Smart Pointers 測試程式
// last update : 20030402
// 適用編譯器：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1) 
//             GNU-C++ 2.95.3-5 (Cygwin special,20010316) 

// cb6[x] 
// cb5[x]: 不接受 friend inline func(param-list) { ... } 

#include "SmartPtr-jj.h"
#include <iostream>

class Widget
{
public:
    void Fun() { std::cout << "hello" << std::endl;  }
}; 

class Printer
{
public:
    void Acquire() { std::cout << "Printer::Acquire" << std::endl;  }
    void Release() { std::cout << "Printer::Release" << std::endl;  }    
}; 


int main()
{
  {	
  // p158 
  // 註：SmartPtrTest 是我在 "SmartPtr-jj.h" 中添加的一個簡易模擬。	
  Loki::SmartPtrTest<Widget> sp(new Widget);
  sp->Fun();
  (*sp).Fun();
  
  // p162
  Loki::SmartPtrTest<Printer> spRes(new Printer);
  spRes->Acquire();
  spRes->Release();  // Printer::Release
          // 分析：spRes-> 傳回一個 Printer*，編譯器再於其上執行 operator->，
          // 因此喚起 Printer::Release() 
  spRes.Release();   // SmartPtr::Release
  }


  using namespace Loki; 
  {
  // p158	
  SmartPtr<Widget> sp(new Widget);
  sp->Fun();
  (*sp).Fun();
  
  // p162
  SmartPtr<Printer> spRes(new Printer);
  spRes->Acquire();  // Printer::Acquire
  spRes->Release();  // Printer::Release
  }	
}

