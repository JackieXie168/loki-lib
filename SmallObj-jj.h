// Loki 侯捷調整版
// 檔名 : SmallObj-jj.h
// 根據 : Loki's SmallObj.h
// 調整者 : jjhou
// 最後更改日期 : 20030402
// 說明：本檔僅為個人修練之用
// 適用：GNU-C++ 3.2 (MinGW-2.0.0-3,special 20020817-1)  
//       GNU-C++ 2.95.3-5 (Cygwin special,20010316)
// 編譯：command line mode 直接以 g++ 編譯，不需任何選項（options）


////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: February 19, 2001

#ifndef SMALLOBJ_INC_
#define SMALLOBJ_INC_

#include "Threads-jj.h"
#include "Singleton-jj.h"
#include <cstddef>
#include <vector>
#include <iostream>  // jjhou

#ifndef DEFAULT_CHUNK_SIZE
#define DEFAULT_CHUNK_SIZE 4096
#endif

#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 64
#endif

namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
// class FixedAllocator
// Offers services for allocating fixed-sized objects
////////////////////////////////////////////////////////////////////////////////

    class FixedAllocator
    {
    // private:
    public:  // jjhou
        struct Chunk
        {
            void Init(std::size_t blockSize, unsigned char blocks);
            void* Allocate(std::size_t blockSize);
            void Deallocate(void* p, std::size_t blockSize);
            void Reset(std::size_t blockSize, unsigned char blocks);
            void Release();
            unsigned char* pData_;
            unsigned char
                firstAvailableBlock_,
                blocksAvailable_;
        };

        // Internal functions
        void DoDeallocate(void* p);
        Chunk* VicinityFind(void* p);

        // Data
        std::size_t blockSize_;
        unsigned char numBlocks_;
        typedef std::vector<Chunk> Chunks;
        Chunks chunks_;
        Chunk* allocChunk_;
        Chunk* deallocChunk_;
        // For ensuring proper copy semantics
        mutable const FixedAllocator* prev_;
        mutable const FixedAllocator* next_;

    public:
        // Create a FixedAllocator able to manage blocks of 'blockSize' size
        explicit FixedAllocator(std::size_t blockSize = 0);
        FixedAllocator(const FixedAllocator&);
        FixedAllocator& operator=(const FixedAllocator&);
        ~FixedAllocator();

        void Swap(FixedAllocator& rhs);

        // Allocate a memory block
        void* Allocate();
        // Deallocate a memory block previously allocated with Allocate()
        // (if that's not the case, the behavior is undefined)
        void Deallocate(void* p);
        // Returns the block size with which the FixedAllocator was initialized
        std::size_t BlockSize() const
        { return blockSize_; }
        // Comparison operator for sorting
        bool operator<(std::size_t rhs) const
        { return BlockSize() < rhs; }
    };

////////////////////////////////////////////////////////////////////////////////
// class SmallObjAllocator
// Offers services for allocating small-sized objects
////////////////////////////////////////////////////////////////////////////////

    class SmallObjAllocator
    {
    public:
        SmallObjAllocator(
            std::size_t chunkSize,
            std::size_t maxObjectSize);

        void* Allocate(std::size_t numBytes);
        void Deallocate(void* p, std::size_t size);

    // private:
    public: // jjhou
        SmallObjAllocator(const SmallObjAllocator&);
        SmallObjAllocator& operator=(const SmallObjAllocator&);

        typedef std::vector<FixedAllocator> Pool;
        Pool pool_;
        FixedAllocator* pLastAlloc_;
        FixedAllocator* pLastDealloc_;
        std::size_t chunkSize_;
        std::size_t maxObjectSize_;
    };

////////////////////////////////////////////////////////////////////////////////
// class SmallObject
// Base class for polymorphic small objects, offers fast
//     allocations/deallocations
////////////////////////////////////////////////////////////////////////////////

    template
    <
        template <class> class ThreadingModel = DEFAULT_THREADING,
        std::size_t chunkSize = DEFAULT_CHUNK_SIZE,
        std::size_t maxSmallObjectSize = MAX_SMALL_OBJECT_SIZE
    >
    class SmallObject : public ThreadingModel<
        SmallObject<ThreadingModel, chunkSize, maxSmallObjectSize> >
    {
    public: // jjhou（原本沒有指定，也就是 private）	
        struct MySmallObjAllocator : public SmallObjAllocator
        {
            MySmallObjAllocator()
            : SmallObjAllocator(chunkSize, maxSmallObjectSize)
            {}
        };
        // The typedef below would make things much simpler,
        //     but MWCW won't like it
        
        //20030402：注意下面這個。我原本 remark 它的#2,#3,#4 個 template 參數。
        // 現暫且開放。同時，我也 remark 了整個 typedef，現暫仍維持 remark 狀態。
        typedef SingletonHolder<MySmallObjAllocator, CreateStatic,
                  DefaultLifetime, ThreadingModel> MyAllocator;

    public:
        static void* operator new(std::size_t size)
        {
#if (MAX_SMALL_OBJECT_SIZE != 0) && (DEFAULT_CHUNK_SIZE != 0)
            //Lock lock;			// jjhou: cb5[o], g32[x]. 暫時封掉
            //lock; // get rid of warning	// jjhou 暫時封掉
            
            std::cout << "in SmallObject::operator new" << std::endl;  // jjhou
            return SingletonHolder<MySmallObjAllocator, CreateStatic,
                PhoenixSingleton>::Instance().Allocate(size);
#else
            return ::operator new(size);
#endif
        }
        static void operator delete(void* p, std::size_t size)
        {        	
#if (MAX_SMALL_OBJECT_SIZE != 0) && (DEFAULT_CHUNK_SIZE != 0)
            //Lock lock;			// jjhou 暫時封掉
            //lock; // get rid of warning		// 奇怪，上一行封掉後為何此行能夠通過編譯？

            std::cout << "in SmallObject::operator delete" << std::endl;  // jjhou
            SingletonHolder<MySmallObjAllocator, CreateStatic,
                PhoenixSingleton>::Instance().Deallocate(p, size);
#else
            ::operator delete(p, size);
#endif
        }
        virtual ~SmallObject() {}      
    };
    
} // namespace Loki

#endif // SMALLOBJ_INC_
