// 《C++設計新思維》（Modern C++ Design）第07章 Smart Pointers 測試程式
// last update : 20030402
// 適用編譯器：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1) 

// cb6[o], cb5[o], 
// g32[o]
// gcc291[x]
// g295[x]: Internal compiler error 
// vc6[x]:template definitions cannot nest

#include <iostream>
#include <string>
#include <cstdlib>  // for malloc()
#include <new>      // for placement new
using namespace std;

// policy class 1
template <class T>
struct OpNewCreator
{
  static T* Create()
  {
    cout << "OpNewCreator::Create()" << endl;
    return new T;
  }
protected:
  ~OpNewCreator() { cout << "OpNewCreator::dtor" << endl; }
};

// policy class 2
template <class T>
struct MallocCreator
{
  static T* Create()
  {
    cout << "MallocCreator::Create()" << endl;
    void* buf = malloc(sizeof(T));
    if (!buf) return 0;
    return new(buf) T;  // placement new, invoke ctor
  }
//protected: // 為了 test6，我把此處改為 public:
  ~MallocCreator() { cout << "MallocCreator::dtor" << endl; }
};

// policy class 3
template <class T>
struct PrototypeCreator
{
  PrototypeCreator(T* pObj = 0)
    : pPrototype_(pObj)
  { }

  T* Create()
  {
    cout << "PrototypeCreator::Create()" << endl;
    return pPrototype_ ? pPrototype_->Clone() : 0;
  }

  T* GetPrototype() { return pPrototype_; }
  void SetPrototype(T* pObj) { pPrototype_ = pObj; }

private:
  T* pPrototype_;
protected:
  ~PrototypeCreator() { 
      cout << "PrototypeCreator::dtor" << endl;  
      if (pPrototype_) delete pPrototype_; 
  }
};



// lib code
// implementing policy classes with "template template parameters"
class Widget;  // Q: 如果這兒是 lib，怎麼能夠要求 lib 知道 user-defined class 呢？
class Gadget;

template<
    template <class /* Created */ >                      // "Created" needless!
    class CreationPolicy = OpNewCreator >                // default policy
class WidgetManager : public CreationPolicy<Widget>      // using policy
{
public:	
  // 下面這個函式只有在 "Prototype policy" 情況下才會被用到，也因此才會被編譯。	
  void SwitchPrototype(Widget* pNewPrototype)
  {
  	CreationPolicy<Widget>& myPolicy = *this;   // jjhou:up-cast
  	delete myPolicy.GetPrototype();
  	myPolicy.SetPrototype(pNewPrototype);
  }	
  
  // 使用同一個 create-policy，產生不同的 object.
  void DoSomething()
  {
        Gadget* pw= CreationPolicy<Gadget>().Create();	
        // 上行產生暫時物件，死亡時會喚起 CreationPolicy<Gadget> 的 dtor.
        // 如果該 dtor 是 protected，實驗便過不了。但我認為目前在 derived class 中，
        // 應該可以喚起 base class's protected method 的啊。
        pw->func();
        delete pw;
  }	
};

//---------------------------------------------------------------

// application code
class Widget
{
public:
  Widget(int x=0, int y=0) : x_(x),y_(y) { }
  ~Widget() { cout << "Widget::dtor" << endl; }
  void func() { cout << '[' << x_ << ',' << y_ << ']' << endl; }
  Widget* Clone() { return new Widget(*this); }
private:
  int x_, y_;
};

class Gadget
{
public:
  Gadget() : str_(string("hello")) { }
  Gadget(const string& str) : str_(str) { }
  ~Gadget() { cout << "Gadget::dtor" << endl; }
  void func() { cout << '[' << str_ << ']' << endl; }
  Gadget* Clone() { return new Gadget(*this); }
private:
  string str_;
};


int main()
{
  { // test1
  typedef WidgetManager< OpNewCreator > myWidgetMgr;
  myWidgetMgr wMsg;
  Widget* pw = wMsg.Create();   // OnNewCreator::Create()
  pw->func();                   // [0,0]
  delete pw;
  }

  { // test2
  typedef WidgetManager< MallocCreator > myWidgetMgr;
  myWidgetMgr wMsg;
  Widget* pw = wMsg.Create();   // MallocCreator::Create()
  pw->func();                   // [0,0]
  delete pw;
  }

  { // test3
  typedef WidgetManager< PrototypeCreator > myWidgetMgr;
  myWidgetMgr wMsg;
  
  wMsg.SetPrototype(new Widget(1,1));
  Widget* pw = wMsg.Create();   // PrototypeCreator<T>::Create()
  pw->func();                   // [1,1]
  delete pw;
  
  wMsg.SetPrototype(new Widget(2,2));   
  // 以上會造成 memory leak。應先 GetPrototype(), 再 delete，再 SetPrototype()
  pw = wMsg.Create();           // PrototypeCreator<T>::Create()
  pw->func();                   // [2,2]  
  delete pw;
  }

  { // test4
  typedef WidgetManager< PrototypeCreator > myWidgetMgr;
  myWidgetMgr wMsg;
  
  wMsg.SetPrototype(new Widget(3,3));
  Widget* pw = wMsg.Create();   // PrototypeCreator<T>::Create()
  pw->func();                   // [3,3]
  delete pw;
  
  wMsg.SwitchPrototype(new Widget(4,4));  // no memory leak
  pw = wMsg.Create();           // PrototypeCreator<T>::Create()
  pw->func();                   // [4,4]  
  delete pw;
  }

  { // test5
  typedef WidgetManager< > myWidgetMgr;  // use default argument
  myWidgetMgr wMsg;
  Widget* pw = wMsg.Create();   // OnNewCreator<T>::Create()
  pw->func();                   // [0,0]
  delete pw;
  }

  { // test6
  typedef WidgetManager< MallocCreator > myWidgetMgr;
  myWidgetMgr wMsg;
  Widget* pw = wMsg.Create();   // MallocCreator::Create()
  pw->func();                   // [0,0]
  wMsg.DoSomething();           // [hello]Gadget::dtor
     // 上行過不了。CB 錯誤訊息是：
     //   Destructor for 'MallocCreator<Gadget>' is not accessible
     // 如果我將 MallocCreator <T> 的 protected dtor 改為 public
     //   則 CB5 便通過了（但 G291 依然無法編譯）。
  delete pw;
  }
}
