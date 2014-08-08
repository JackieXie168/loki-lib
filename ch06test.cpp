// 《C++設計新思維》（Modern C++ Design）第06章 Singleton 測試程式
// last update : 20030402
// 適用編譯器：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1) 
//             GNU-C++ 2.95.3-5 (Cygwin special,20010316) 

#include "Singleton-jj.cpp"  
#include <iostream>
#include <string>
using namespace std;                             
using namespace Loki; 


// p154
class KeyboardImpl
{
public:
  KeyboardImpl() { cout << "Keyboard created!" << endl; }
  ~KeyboardImpl() { cout << "Keyboard destroied!" << endl; }	
  void demo() { cout << "Keyboard" << endl; }	
};	

class DisplayImpl
{
public:
  DisplayImpl() { cout << "Display created!" << endl; }
  ~DisplayImpl() { cout << "Display destroied!" << endl; }	
  void demo() { cout << "Display" << endl; }	  
};

class LogImpl
{
public:
  LogImpl() { cout << "Log created!" << endl; }
  ~LogImpl() { cout << "Log destroied!" << endl; }	
  void demo() { cout << "Log" << endl; }	  
};

// p155
inline unsigned int GetLongevity(KeyboardImpl*) { return 1; }
inline unsigned int GetLongevity(DisplayImpl*) { return 3; }
inline unsigned int GetLongevity(LogImpl*) { return 2; }

// p155, 使用 SingletonWithLongevity
typedef SingletonHolder<KeyboardImpl, CreateUsingNew, SingletonWithLongevity> Keyboard;
typedef SingletonHolder<DisplayImpl, CreateUsingNew, SingletonWithLongevity> Display;
typedef SingletonHolder<LogImpl, CreateUsingNew, SingletonWithLongevity> Log; 

// 使用 default argument (DefaultLitetime)
typedef SingletonHolder<KeyboardImpl> Keyboard2;
typedef SingletonHolder<DisplayImpl> Display2;
typedef SingletonHolder<LogImpl> Log2; 

// 使用 PhoenixSingleton
typedef SingletonHolder<KeyboardImpl, CreateUsingNew, PhoenixSingleton> Keyboard3;
typedef SingletonHolder<DisplayImpl, CreateUsingNew, PhoenixSingleton> Display3;
typedef SingletonHolder<LogImpl, CreateUsingNew, PhoenixSingleton> Log3; 



int main(int argc, char* argv[])
{
    if (argc != 2) {
        cout << "please select Lifetime Policy(P or D or L) as command line option" << endl;
        exit(1);
    }
	
    if (argv[1][0] == 'L')  	// Longevity
    {	
        Log::Instance().demo();
        Display::Instance().demo();    
        Keyboard::Instance().demo();
      /* output:
        Log created!
        Log
        Display created!
        Display
        Keyboard created!
        Keyboard
        
        Display destroied!  我發現 longevity 數值愈大愈快被銷毀（和書上說的不一樣）
        Log destroied!
        Keyboard destroied!
      */      
    }
    else if (argv[1][0] == 'D') 	// Default
    {
        Log2::Instance().demo();
        Display2::Instance().demo();    
        Keyboard2::Instance().demo();    
      /* output:
	Log created!
	Log
	Display created!
	Display
	Keyboard created!
	Keyboard
	
	Keyboard destroied!
	Display destroied!
	Log destroied!
      */  
    }  
    else if (argv[1][0] == 'P') 	// Phoenix
    {
        Log3::Instance().demo();
        Display3::Instance().demo();    
        Keyboard3::Instance().demo();    
      /* output:
       	Log created!
	Log
	Display created!
	Display
	Keyboard created!
	Keyboard
	
	Keyboard destroied!
	Display destroied!
	Log destroied!
      */    
    }        
}
