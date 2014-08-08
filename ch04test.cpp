// 《C++設計新思維》（Modern C++ Design）第04章 Small Object Allocator 測試程式
// last update : 20030402
// 適用編譯器：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1) 
//             GNU-C++ 2.95.3-5 (Cygwin special,20010316) 

// cb6[x]: Error E2034 C:\Borland\CBuilder6\Include\stl\_algobase.c 371:
// cb5[o]

#include "SmallObj-jj.cpp" // 為求測試方便及簡化的權宜之計，其中含入 "SmallObj-jj.h"
    // 本應含入 "SmallObj-jj.h" 並與 SmallObj-jj.obj (來自 SmallObj-jj.cpp) 聯結
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
using namespace Loki;


// jjhou: 我改變了 "SmallObj.h" 中的 data members access level，因此才能在此觀察它們
void LokiAllocator_dissecting(SmallObjAllocator& myAlloc)
{
    cout << myAlloc.chunkSize_   << ' '
         << myAlloc.pool_.size() << endl;

    SmallObjAllocator::Pool::iterator ite1 = myAlloc.pool_.begin();
    SmallObjAllocator::Pool::iterator ite2 = myAlloc.pool_.end();
    for (; ite1!=ite2; ++ite1) {
         cout << ite1->blockSize_ << ' '
              << (int)ite1->numBlocks_ << ' '
              << ite1->chunks_.size() << ' ';
              
         cout << "allocChunk:" << ite1->allocChunk_ << ' ';
         cout << "deallocChunk:" << ite1->deallocChunk_ << ' ';  
         
         FixedAllocator::Chunks::iterator ite3 = ite1->chunks_.begin();
         FixedAllocator::Chunks::iterator ite4 = ite1->chunks_.end();         
             
         for (; ite3!=ite4; ++ite3) {
              cout << "current:" << &(*ite3) << ' ';
              cout << (int)ite3->firstAvailableBlock_ << ' '
                   << (int)ite3->blocksAvailable_ << ' '
                   << endl;     

              /* 印出每個 block 最前 1byte 所記錄的值 (as a index or ptr)     
              for (int i=0; i< (int)ite1->numBlocks_; ++i)
                   cout << (int)(*(ite3->pData_+(i*ite1->blockSize_))) << ' ';
              cout << endl;     
              */     
         }
    }
}

class C : public SmallObject<>
{
private:
    int x,y;	
};	


int main()
{
    SmallObjAllocator myAlloc(2048,256);

    void* p1 = (void*)myAlloc.Allocate(20);
    void* p2 = (void*)myAlloc.Allocate(40);
    void* p3 = (void*)myAlloc.Allocate(300); // > 256
    void* p4 = (void*)myAlloc.Allocate(64);
    void* p5 = (void*)myAlloc.Allocate(64);
    void* p6 = (void*)myAlloc.Allocate(64);
    void* p7 = (void*)myAlloc.Allocate(64);

    LokiAllocator_dissecting(myAlloc);
    // 2048 3
    // 20 204 (=4096/20)  1 1 203
    // 40 102 (=4096/40)  1 1 101
    // 64 64  (=4096/64)  1 4 60

    void* ptr[100];
    for (int i=0; i< 65; ++i)
        ptr[i] = (void*)myAlloc.Allocate(64);

    LokiAllocator_dissecting(myAlloc);
    // 2048 3
    // 20 204 (=4096/20)  1 1 203
    // 40 102 (=4096/40)  1 1 101
    // 64 64  (=4096/64)  2 64 0
    //                      5 59
    
    
    myAlloc.Deallocate(p5,64);
    LokiAllocator_dissecting(myAlloc);    

/* output: 
2048 3
20 204 1 allocChunk:006734C4 deallocChunk:006734C4 current:006734C4 1 203
40 102 1 allocChunk:006748BC deallocChunk:006748BC current:006748BC 1 101
64 64  2 allocChunk:00675DEC deallocChunk:00675DE4 current:00675DE4 1 1
                                                   current:00675DEC 5 59
*/    
    
    myAlloc.Deallocate(p6,64);
    LokiAllocator_dissecting(myAlloc);     
/* output:    
2048 3
20 204 1 allocChunk:006734C4 deallocChunk:006734C4 current:006734C4 1 203
40 102 1 allocChunk:006748BC deallocChunk:006748BC current:006748BC 1 101
64 64  2 allocChunk:00675DEC deallocChunk:00675DE4 current:00675DE4 2 2
                                                   current:00675DEC 5 59
*/
       
    myAlloc.Deallocate(p4,64);
    LokiAllocator_dissecting(myAlloc);    
/* output:     
20 204 1 allocChunk:006734C4 deallocChunk:006734C4 current:006734C4 1 203
40 102 1 allocChunk:006748BC deallocChunk:006748BC current:006748BC 1 101
64 64  2 allocChunk:00675DEC deallocChunk:00675DE4 current:00675DE4 0 3
                                                   current:00675DEC 5 59    
*/     
    
    myAlloc.Deallocate(p7,64);
    LokiAllocator_dissecting(myAlloc);    
/* output:     
2048 3
20 204 1 allocChunk:006734C4 deallocChunk:006734C4 current:006734C4 1 203
40 102 1 allocChunk:006748BC deallocChunk:006748BC current:006748BC 1 101
64 64  2 allocChunk:00675DEC deallocChunk:00675DE4 current:00675DE4 3 4
                                                   current:00675DEC 5 59
*/     
        
    myAlloc.Deallocate(p1,20);
    myAlloc.Deallocate(p2,40);
    myAlloc.Deallocate(p3,300);
    for (int i=0; i< 65; ++i)
        myAlloc.Deallocate(ptr[i], 64);
    LokiAllocator_dissecting(myAlloc);

/* output: 
2048 3
20 204 1 allocChunk:006734C4 deallocChunk:006734C4 current:006734C4 0 204
40 102 1 allocChunk:006748BC deallocChunk:006748BC current:006748BC 0 102
64 64  1 allocChunk:00675DE4 deallocChunk:00675DE4 current:00675DE4 4 64
*/

                                              // cb5    g32   g295            
    cout << sizeof(SmallObject<>) << endl;    // 16   	4     8
    cout << sizeof(C) << endl;                // 24	12    16
    // CB5 的表現豈不是太可笑了？為獲得 small object 反而讓每個 C object 變大那麼多。
    // 侯捷《池內春秋》表現如何 — 每個 object 多 4 bytes。
    // G32 的表現不錯。
    
    C c1,c2,c3;
    C* pc1 = new C; 	// "in SmallObject::operator new
    C* pc2 = new C; 	// "in SmallObject::operator new   
    
    LokiAllocator_dissecting(
        SingletonHolder<SmallObject<>::MySmallObjAllocator, CreateStatic,
                        PhoenixSingleton>::Instance());
/* 
cb5 output: 
4096 1
24 170 1 allocChunk:0067813C deallocChunk:0067813C current:0067813C 2 168

g32 output:
4096 1
12 255 1 allocChunk:0x780398 deallocChunk:0x780398 current:0x780398 2 253

g295 output:
4096 1
16 255 1 allocChunk:0x991360 deallocChunk:0x991360 current:0x991360 2 253
*/                        
                        
    delete pc1;		// "in SmallObject::operator delete 
    delete pc2;         // "in SmallObject::operator delete 
}
