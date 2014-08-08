// 《C++設計新思維》（Modern C++ Design）第09章 AbstractFactory 測試程式
// last update : 20030402
// 適用編譯器：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1) 
//             GNU-C++ 2.95.3-5 (Cygwin special,20010316) 

#include "AbstractFactory-jj.h"
#include "TypeList-jj.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Loki;
using namespace Loki::TL;

//------------------------------------
// 以下做出 p220 的 class hierarchy
class Enemy
{
public:
  virtual ~Enemy() { }	
  // 如果上式 =0，會出現 undefined reference to Enemy::~Enemy()。
  
  virtual void declare()=0;	  	
};
//------------------------------------	
class Soldier : public Enemy
{
public:
  virtual ~Soldier() { }	
};
class SillySoldier : public Soldier
{
public:
  virtual ~SillySoldier() { }	
  virtual void declare() { cout << "I am SillySoldier" << endl; }	
};
class BadSoldier : public Soldier
{
public:
  virtual ~BadSoldier() { }	
  virtual void declare() { cout << "I am BadSoldier" << endl; }	
};
//------------------------------------	
class Monster : public Enemy
{
public:
  virtual ~Monster() { }  		
};
class SillyMonster : public Monster
{
public:
  virtual ~SillyMonster() { }	
  virtual void declare() { cout << "I am SillyMonster" << endl; }	  	
};
class BadMonster : public Monster
{
public:
  virtual ~BadMonster() { }
  virtual void declare() { cout << "I am BadMonster" << endl; }	  		
};	
//------------------------------------	
class SuperMonster : public Enemy
{
public:
  virtual ~SuperMonster() { }		  	
};
class SillySuperMonster : public SuperMonster
{
public:
  virtual ~SillySuperMonster() { }
  virtual void declare() { cout << "I am SillySuperMonster" << endl; }    		
};
class BadSuperMonster : public SuperMonster
{
public:
  virtual ~BadSuperMonster() { }
  virtual void declare() { cout << "I am BadSuperMonster" << endl; }  		
};
//-------------------------------------



class GameApp
{
public:	
  // p224
  typedef AbstractFactory<TYPELIST_3(Soldier, Monster, SuperMonster)> 
      AbstractEnemyFactory;
    
  // p226  
  typedef ConcreteFactory<AbstractEnemyFactory, 
                          OpNewFactoryUnit,
                          TYPELIST_3(SillySoldier,SillyMonster,SillySuperMonster)>
      EasyLevelEnemyFactory;

  // similar to p226
  typedef ConcreteFactory<AbstractEnemyFactory, 
                          OpNewFactoryUnit,
                          TYPELIST_3(BadSoldier,BadMonster,BadSuperMonster)>
      DieHardLevelEnemyFactory; 

  // similar to p222
  void SelectLevel(char option)
  {
      if (option == '1') {
          pFactory_ = new EasyLevelEnemyFactory;     	
      }
      else if (option == '2') {
          pFactory_ = new DieHardLevelEnemyFactory;         	
      }	      	
      else {
          cout << "please select level(1 or 2) at command line" << endl;         	
      }	            
  }	
  
  void play()
  {
    // similar to p224. prepare the players...	
    Monster* pMon1 = pFactory_->Create<Monster>();
    Monster* pMon2 = pFactory_->Create<Monster>();  
    SuperMonster* pSuperMon1 = pFactory_->Create<SuperMonster>();
    Soldier* pSoldier1 = pFactory_->Create<Soldier>();  
    Soldier* pSoldier2 = pFactory_->Create<Soldier>();  
    
    //declare themself  
    pMon1->declare(); 	
    pMon2->declare(); 	
    pSuperMon1->declare(); 	
    pSoldier1->declare(); 	
    pSoldier2->declare(); 
    
    // and then playing...	                
  }	
  
private:
  AbstractEnemyFactory* pFactory_;  // 用來持有 concrete factory.
                                    //   由 SelectLevel() 設定之		
};
	
//------------------------------------	
int main(int argc, char* argv[])
{
    if (argc != 2) {
        cout << "please select level(1 or 2) at command line" << endl;    
        exit(1);
    }         	
      	
    GameApp myGame;
    myGame.SelectLevel(argv[1][0]);
    myGame.play();

/* 
run. case1 :
D:\mcd\lokijj\g32>ch9test 1
I am SillyMonster
I am SillyMonster
I am SillySuperMonster
I am SillySoldier
I am SillySoldier

run case 2 :
D:\mcd\lokijj\g32>ch9test 2
I am BadMonster
I am BadMonster
I am BadSuperMonster
I am BadSoldier
I am BadSoldier
*/    
}  
