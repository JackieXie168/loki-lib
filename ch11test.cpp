// 《C++設計新思維》（Modern C++ Design）第11章 Multimethods 測試程式
// last update : 20030402
// 適用編譯器：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1) 
//             GNU-C++ 2.95.3-5 (Cygwin special,20010316) 

#include "MultiMethods-jj.h"
#include "Typelist-jj.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Loki;

//--------------------------------------------------
// 以下設計一個 class hierarchy，內有 Shape, Rect, Elli, Poly 四個 classes
class Shape 
{ 
public:
  virtual ~Shape() { };
  
};

// 以下在 G32 中竟然不能命名為 Rectangle, 多麼可笑
class Rect : public Shape  
{ 
public:
  
};

// 以下在 G32 中竟然不能命名為 Ellipse，多麼可笑
class Elli : public Shape 
{ 
public:
  
};

class Poly : public Shape 
{ 
public:
  
};
//--------------------------------------------------


// 用於 multiple dispatch 的 Executor
class HatchingExecutor
{
public:
  // 以下少了一個都不行。因為 Loki 為我們完成 "暴力式 multi-methods" 時
  // 會產生「呼叫以下函式」的代碼。
  void Fire(Rect&, Rect&)   { cout << "Fire(Rect&, Rect&)" << endl; }	 
  void Fire(Rect&, Elli&)   { cout << "Fire(Rect&, Elli&)" << endl; }	
  void Fire(Rect&, Poly&)   { cout << "Fire(Rect&, Poly&)" << endl; }	
  void Fire(Elli&, Poly&)   { cout << "Fire(Elli&, Poly&)" << endl; }	
  void Fire(Elli&, Elli&)   { cout << "Fire(Elli&, Elli&)" << endl; }	
  void Fire(Elli&, Rect&)   { cout << "Fire(Elli&, Rect&)" << endl; }	
  void Fire(Poly&, Poly&)   { cout << "Fire(Poly&, Poly&)" << endl; }	
  void Fire(Poly&, Rect&)   { cout << "Fire(Poly&, Rect&)" << endl; }	
  void Fire(Poly&, Elli&)   { cout << "Fire(Poly&, Elli&)" << endl; }	
    
  // Error handling
  void OnError(Shape&, Shape&) { cout << "OnError(Shape&, Shape&)" << endl; }
};            

	
//------------------------------------	
int main(int argc, char* argv[])
{
    // p271. 定義一個 static（暴力式的）Dispatcher
    // Rect, Elli, Poly 為互相影響的三個 classes。
    typedef StaticDispatcher<HatchingExecutor, true, Shape, 
                             TYPELIST_3(Rect,Elli,Poly)> Dispatcher;
    Rect r1,r2;
    Elli e1,e2;
    Poly p1,p2;                         	
    HatchingExecutor exec;
    
                                        // output:
    Dispatcher::Go(r1,e1,exec);		// Fire(Rect&, Elli&)
    Dispatcher::Go(r1,r2,exec);		// Fire(Rect&, Rect&)
    Dispatcher::Go(r1,p1,exec);		// Fire(Rect&, Poly&)
    Dispatcher::Go(p1,p2,exec);		// Fire(Poly&, Poly&)
    Dispatcher::Go(p1,e1,exec);		// Fire(Poly&, Elli&)
    Dispatcher::Go(p1,r1,exec);		// Fire(Poly&, Rect&)
    Dispatcher::Go(e1,e2,exec);         // Fire(Elli&, Elli&)
    Dispatcher::Go(e1,r1,exec);   	// Fire(Elli&, Rect&)
    Dispatcher::Go(e1,p1,exec);  	// Fire(Elli&, Poly&)     
    
    // 本程式尚未測試 Static Dispatcher 以外的其他 Dispatcher。                  
}  
