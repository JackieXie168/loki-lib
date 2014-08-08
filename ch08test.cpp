// 《C++設計新思維》（Modern C++ Design）第08章 Object Factories 測試程式
// last update : 20030402
// 適用編譯器：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1) 
//             GNU-C++ 2.95.3-5 (Cygwin special,20010316) 

#include "Factory-jj.h"
#include "Singleton-jj.h"  // for test(5), p215
#include "Functor-jj.h"    // for test(5), p215
#include <string>
#include <iostream>
#include <typeinfo>  // for typeid() operator
using namespace std;

//-----------------------------------------
// p201，產生一個 Shape class hierarchy
class Shape
{
public:
	virtual void Draw() const = 0;
	virtual Shape* Create() const = 0;
	
	// p213
	// for CloneFactory
	/*
	Shape* Clone() const   
	{
	  Shape* pClone = DoClone();
	  assert(typeid(*pClone) == typeid(*this));
	  return pClone;	
	}
	*/	
private:
        // for CloneFactory，目前沒用上
        // virtual Shape* DoClone() const = 0;  	
};

class Line : public Shape
{
public:
	virtual void Draw() const { cout << "Line" << endl;  }	
	virtual Shape* Create() const { cout << "Line::Create" << endl; return new Line; }	
};

class Circle : public Shape
{
public:
	virtual void Draw() const { cout << "Circle" << endl;  }
	virtual Shape* Create() const { cout << "Circle::Create" << endl; return new Circle; }			
};

class Square : public Shape
{
public:
	virtual void Draw() const { cout << "Square" << endl;  }
	virtual Shape* Create() const { cout << "Square::Create" << endl; return new Square; }				
};
//-----------------------------------------



//-----------------------------------------
// 這些相當於 MFC 中以 DECLARE_xxx, IMPLEMENT_xxx 做出來的 code
// 不過 MFC 是做在 class 內成為 member functions
// p204
Shape* CreateLine()  {
    cout << "CreateLine" << endl;
    return new Line;
}
const int LINE = 1;

Shape* CreateCircle()  {
    cout << "CreateCircle" << endl;
    return new Circle;
}
const int CIRCLE = 2;
	
Shape* CreateSquare()  {
    cout << "CreateSquare" << endl;
    return new Square;
}
const int SQUARE = 3;
//-----------------------------------------



// for CloneFactory
// 我的疑問：這是正規作法嗎？要求這麼寫，不是太可怕了嗎？
// 所以，以 member functions 做為 creation function，才是正途。
Shape* CloneLine(const Shape* line) {
	// 以下喚起 copy ctor
	return new Line(*dynamic_cast<const Line*>(line));
}
Shape* CloneCircle(const Shape* circle) {
	// 以下喚起 copy ctor	
	return new Circle(*dynamic_cast<const Circle*>(circle));
}
Shape* CloneSquare(const Shape* square) {
	// 以下喚起 copy ctor	
	return new Square(*dynamic_cast<const Square*>(square));
}



//----------------------------------------------------------
int main()
{
  { // (1) 以 int 為 ID, 以 namespace functions 為 creation function	
  Loki::Factory<Shape, int> ShapeFactory;
  bool registered;
  
  registered = ShapeFactory.Register(LINE, CreateLine); 
  registered = ShapeFactory.Register(CIRCLE, CreateCircle);	
  registered = ShapeFactory.Register(SQUARE, CreateSquare);	
	 	
  Shape* p1 = ShapeFactory.CreateObject(LINE); 
  p1->Draw();	
  Shape* p2 = ShapeFactory.CreateObject(CIRCLE); 
  p2->Draw();
  Shape* p3 = ShapeFactory.CreateObject(SQUARE); 
  p3->Draw();
  /* output:
  CreateLine
  Line
  CreateCircle
  Circle
  CreateSquare
  Square
  */
  
  delete p1;
  delete p2;
  delete p3;
  }
  
  { // (2) 以 std::string 為 ID, 以 namespace functions 為 creation function		
  Loki::Factory<Shape,std::string> ShapeFactory;
  bool registered;
  
  registered = ShapeFactory.Register(string("Line"), CreateLine); 
  registered = ShapeFactory.Register(string("Circle"), CreateCircle);	
  registered = ShapeFactory.Register(string("Square"), CreateSquare);	
	 	
  Shape* p1 = ShapeFactory.CreateObject(string("Line")); 
  p1->Draw();	
  Shape* p2 = ShapeFactory.CreateObject(string("Circle")); 
  p2->Draw();
  Shape* p3 = ShapeFactory.CreateObject(string("Square")); 
  p3->Draw();  
  /* output:
  CreateLine
  Line
  CreateCircle
  Circle
  CreateSquare
  Square
  */ 
  
  delete p1;
  delete p2;
  delete p3; 
  }	 

  { // (3) 以 std::string 為 ID, 以 member functions 為 creation function		
  typedef Shape* (Shape::*ShapeCreator)();  
  Loki::Factory<Shape, std::string, ShapeCreator>  ShapeFactory;
  
  // 我的問題：如何才能夠以 member functions 做為 ProductCreator？
  // 在 derived class- 和 base class- member functions 之間似乎沒有關聯？
  // 但 MFC 的確是以 member function 做為 creator 呢。
  // 註：MFC 是以轉型來完成。我想這裡也一樣地可以將 
  //     pointer to member function of derived class 轉型為
  //     pointer to member function of base class. ---------------------- (a)
  //     也就是把 Shape* (Line::*)() 轉型為 Shape* (Shape::*)()
  //     也把 Shape* (Circle::*)()   轉型為 Shape* (Shape::*)()
  //     也把 Shape* (Square::*)()   轉型為 Shape* (Shape::*)()
  //     另一個方法是如 test(5)，以 Functor of member function 為 ProductCreator. 
  //
  bool registered;
  
  registered = ShapeFactory.Register(string("Line"),   (ShapeCreator)(&Line::Create)); 
  registered = ShapeFactory.Register(string("Circle"), (ShapeCreator)(&Circle::Create));	
  registered = ShapeFactory.Register(string("Square"), (ShapeCreator)(&Square::Create));	
/* output:
  Shape* p1 = ShapeFactory.CreateObject(string("Line")); 
  p1->Draw();	
  Shape* p2 = ShapeFactory.CreateObject(string("Circle")); 
  p2->Draw();
  Shape* p3 = ShapeFactory.CreateObject(string("Square")); 
  p3->Draw(); 
  
  以上將會失敗，因為在 map 中找到的 'value' 是個 plain pointer to member function，
  喚起它時必須使用 operator->*。而 "Factory-jj.h" 中的 CreateObject() 內並非如此。 
  此外，也沒有機會做上述 (a) 式的逆轉動作。 	 	
*/
  }	

  { // (4) test CloneFactory	
  Loki::CloneFactory<Shape> ShapeCloneFactory;
  // 我的問題：如何才能夠以 member functions 做為 ProductCreator？
  // 在 derived class- 和 base class- member functions 之間似乎沒有關聯？  
  // MFC 似乎沒有 CloneFactory
  
  bool registered;
    
  registered = ShapeCloneFactory.Register(typeid(Line), CloneLine); 
  registered = ShapeCloneFactory.Register(typeid(Circle), CloneCircle);	
  registered = ShapeCloneFactory.Register(typeid(Square), CloneSquare);	
  	
  Line line;
  Circle circle;
  Square square;
  	 	
  Shape* p1 = ShapeCloneFactory.CreateObject(&line); 
  p1->Draw();	
  Shape* p2 = ShapeCloneFactory.CreateObject(&square); 
  p2->Draw();
  Shape* p3 = ShapeCloneFactory.CreateObject(&circle); 
  p3->Draw();
  /* output:
  Line
  Square
  Circle
  */  
  
  delete p1;
  delete p2;
  delete p3;  
  }

  { // (5) p215
    // 以 std::string 為 ID, 以 Functor of member functions 為 creation function		
    // 並搭配 singleton pattern
    // 前頭需 #include "Functor-jj.h"
    //        #include "Singleton-jj.h"
  typedef Loki::SingletonHolder< Loki::Factory< Shape,
                                                std::string, 
                                                Loki::Functor<Shape*> > >         
            ShapeFactory;
  bool registered;  // 把這行移到以下 (b) 之上一行，G32 竟然編譯不過。莫名其妙。        
  
  // 前期作業，首先測試 Functor of member function
  Line line;
  Circle circle;
  Square square;
  Loki::Functor<Shape*> cmd1(&line,   &Line::Create);  
  Loki::Functor<Shape*> cmd2(&circle, &Circle::Create);  
  Loki::Functor<Shape*> cmd3(&square, &Square::Create);      
  Shape* p1 = cmd1();
  p1->Draw();
  Shape* p2 = cmd2();
  p2->Draw();
  Shape* p3 = cmd3();
  p3->Draw();
  /* output:
  Line::Create
  Line
  Circle::Create
  Circle
  Square::Create
  Square
  */    
  
  delete p1;
  delete p2;
  delete p3;
  
  // 測試 Factory 
  registered = ShapeFactory::Instance().Register(string("Line"), cmd1);  // ---(b)
  registered = ShapeFactory::Instance().Register(string("Circle"), cmd2);	
  registered = ShapeFactory::Instance().Register(string("Square"), cmd3);	
/*
以上和 MFC 的 CRuntimeClass 比較檢討：
  這裡的 create function 並非 static，所以為它製作 Functor 時你得傳入一個 object。
    MFC 的 create function 則為 static，較好。
  如果令 create function 的名稱固定（例如都為 OnCreate），則可根據 class name， 
    以 macro 自行產生代碼。此即 MFC 的 DECLARE_, IMPLEMENT_ macros 的功用。  
/* 
以下數行原本都編譯不了。原始錯誤訊息如下：
../Factory-jj.h: In member function 
AbstractProduct* Loki::Factory<AbstractProduct, 
                               IdentifierType,
                               ProductCreator>
                       ::CreateObject(const IdentifierType&) 
   [with AbstractProduct = Shape, 
    IdentifierType = std::string, 
    ProductCreator = Loki::Functor<Shape*, Loki::NullType>]':
../ch8test.cpp:256:   instantiated from here
../Factory-jj.h:65: passing
const Loki::Functor<Shape*, Loki::NullType>' as `this' argument of
   R Loki::Functor<R, TList>::operator()() 
   [with R = Shape*, 
    TList = Loki::NullType]' 
  discards qualifiers
  
以下是整理後的錯誤訊息：  
In member function  
    Shape* Loki::Factory<Shape, 
                         std::string,
                         Loki::Functor<Shape*, Loki::NullType>>
                 ::CreateObject(const std::string&)                   
../ch8test.cpp:256:   instantiated from here
../Factory-jj.h:65: passing                
    const Loki::Functor<Shape*, Loki::NullType>                 
as `this' argument of  
    Shape* Loki::Functor<Shape*, Loki::NullType>::operator()() 
discards qualifiers

於是檢查 Factory-jj.h，發現這樣的代碼：
        AbstractProduct* CreateObject(const IdentifierType& id)
        {
 note--->   typename IdToProductMap::const_iterator i = associations_.find(id);
            if (i != associations_.end())
            {
                return (i->second)();
            }
            // return OnUnknownType(id);
        }
把 IdToProductMap::const_iterator 改為 IdToProductMap::iterator，就 OK 了。
亦可見 "Factory-jj.h" 中更詳細的說明。
*/  

  p1 = ShapeFactory::Instance().CreateObject(string("Line")); 
  p1->Draw();	
  p2 = ShapeFactory::Instance().CreateObject(string("Circle")); 
  p2->Draw();
  p3 = ShapeFactory::Instance().CreateObject(string("Square")); 
  p3->Draw(); 
  /* output:
  Line::Create
  Line
  Circle::Create
  Circle
  Square::Create
  Square
  */    
  
  delete p1;
  delete p2;
  delete p3;  
  }
}
