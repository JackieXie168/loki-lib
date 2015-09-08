/******************************************************************************
*******************************************************************************
*******************************************************************************

    ferris loki extensions
    Copyright (C) 2003 Ben Martin

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    For more details see the COPYING file in the root directory of this
    distribution.

    $Id: Extensions.hh,v 1.6 2010/11/14 21:45:41 ben Exp $

*******************************************************************************
*******************************************************************************
******************************************************************************/

#include <sigc++/signal.h>
#include <sigc++/object.h>
#include <loki/SmartPtr.h>
#include <loki/SmallObj.h>

#ifndef _ALREADY_INCLUDED_FERRIS_LOKI_EXTENSIONS_H_
#define _ALREADY_INCLUDED_FERRIS_LOKI_EXTENSIONS_H_

namespace Ferris
{
    class ContextStreamMemoryManager;
};

namespace Loki
{
    // This helps Cedet autocomplete
    // because using the real Loki::SmartPtr is
    // a bit too much.
    template < typename T >
    class CedetSmartPtr
    {
    public:
        CedetSmartPtr();
        T* operator->();
        T& operator*();
    };
};

namespace FerrisLoki
{
    
    template <class P>
    struct FerrisExSmartPointerChecker
    {
        FerrisExSmartPointerChecker() {}
        template <class P1> FerrisExSmartPointerChecker(const FerrisExSmartPointerChecker<P1>&) {}
        template <class P1> FerrisExSmartPointerChecker(const Loki::NoCheck<P1>&) {}
        template <class P1> FerrisExSmartPointerChecker(const Loki::AssertCheck<P1>&) {}
        
        static void OnDefault(const P&) {}
        static void OnInit(const P&) {}
        static void OnDereference(P val)
            {
#ifdef FERRISLOKI_SMARTPTR_SHOULD_ASSERT
                if( !val )
                {
                    const int arraysz = 500;
                    void* array[arraysz];
                    size_t size;

                    size = backtrace( array, arraysz );
                    backtrace_symbols_fd( array, size, STDERR_FILENO );
                    
                    assert(val);
                }
#endif
            }
        static void Swap(FerrisExSmartPointerChecker&) {}
    };
    
    /**
     * SmartPtr<> policy class [OwnershipPolicy] for Handlable objects.
     * This class implements a intrusive reference count like the COMRefCounted
     * policy in the Modern C++ design book.
     */
    template <class P>
    class FerrisExRefCounted
    {
    public:
    
        FerrisExRefCounted() {}
        FerrisExRefCounted( const FerrisExRefCounted& r) {}
        template <class U> FerrisExRefCounted(const FerrisExRefCounted<U>&) {}

        /**
         * Create a new handle
         */
        P Clone(const P& val)
            {
                if( val )
                {
                    val->AddRef();
                }
                return val;
            }
        
        /**
         * Release a reference. This may trigger a getClosureSignal() to fire due to
         * the final reference being dropped.
         *
         * @param val Object that we are releasing an intrusive reference to
         * @return true if the object should die.
         */
        bool Release(const P& val)
            {
                if( !val )
                {
                    return false;
                }
            
                int v = val->AddRef();
                if( v == 2 )
                {
                    val->private_AboutToBeDeleted();
                }
                v = val->Release();
                
                v = val->Release();
                if( !v )
                {
                    /*
                     * Time to die amigo
                     */
                    return true;
                }

                return false;
            }
        
        enum { destructiveCopy = false };
        static void Swap(FerrisExRefCounted&)
            {}
    };
    
    /**
     * Base class for all classes that have handles using the FerrisExRefCounted<>
     * policy.
     */
    class Handlable
        :
        public sigc::trackable
    {
        // memory management
        friend class ::Ferris::ContextStreamMemoryManager;

    public:

        typedef sigc::signal1< void, Handlable* > GenericCloseSignal_t;

    public: // having the ref_count_t exposed makes language bindings easier.

        /**
         * Type for reference counts. This could be 16/32/64 bits.
         */
        typedef uint32_t ref_count_t;

    public: // NB: testing only
        /**
         * shared reference count for all handles that point to this object.
         * Don't access outside object
         */
        ref_count_t ref_count;

        /**
         * Don't access outside object
         */
        GenericCloseSignal_t GenericCloseSignal;

        /**
         * Used internally to notify the object that it is about to be reclaimed
         */
        virtual void private_AboutToBeDeleted();
        
    public:

        Handlable();
        virtual ~Handlable();

        virtual ref_count_t AddRef();
        virtual ref_count_t Release();
        ref_count_t getReferenceCount();
        GenericCloseSignal_t& getGenericCloseSig();

    };


    /**
     * Emits a signal before last reference is dropped.
     */
    template <class T>
    class PtrHandlableSigEmitter
        :
        public sigc::trackable
    {
    public:
        typedef PtrHandlableSigEmitter<T> _Self;
        typedef PtrHandlableSigEmitter<T> ThisClass;
    
        typedef T* PointerType;   // type returned by operator->
        typedef T& ReferenceType; // type returned by operator*

        /**
         * A signal that is emitted when the object is about to die.
         */
        typedef sigc::signal1< void, ReferenceType > CloseSignal_t;
    
        CloseSignal_t& getCloseSig()
            {
                return CloseSignal;
            }
    
        void EmitClose( Handlable* )
            {
                getCloseSig().emit( (PointerType)this );
            }

        PtrHandlableSigEmitter( Handlable* han )
            {
                han->getGenericCloseSig().connect(slot(this, &ThisClass::EmitClose));
            }

        virtual ~PtrHandlableSigEmitter()
            {
            }

    private:
        CloseSignal_t CloseSignal;
    };

    template <class T>
    typename T::_Self::CloseSignal_t&
    getCloseSig( T& obj )
    {
        typename T::_Self * hook = &obj;
        return hook->getCloseSig();
    }


    /**
     * Very much like Loki::DefaultSPStorage except that 
     * DefaultSPStorage(const StoredType& p) : pointee_(p) {}
     * will bump the reference count so that code like the following
     * works
     * ContextSubType* sc = new ContextSubType(); // 1
     * fh_context c = sc;                         // 2
     * the 'c' var will bump the reference count so that when it goes
     * out of scope then the rc will return to what it was on the line (1)
     */
    template <class T>
    class FerrisExSmartPtrStorage
    {
    public:
        typedef T* StoredType;    // the type of the pointee_ object
        typedef T* InitPointerType; /// type used to declare OwnershipPolicy type.
        typedef T* PointerType;   // type returned by operator->
        typedef T& ReferenceType; // type returned by operator*
        
        FerrisExSmartPtrStorage() : pointee_(Default()) 
            {}

        // The storage policy doesn't initialize the stored pointer 
        //     which will be initialized by the OwnershipPolicy's Clone fn
        FerrisExSmartPtrStorage(const FerrisExSmartPtrStorage&)
            {}

        template <class U>
        FerrisExSmartPtrStorage(const FerrisExSmartPtrStorage<U>&) 
            {}
        
        FerrisExSmartPtrStorage(const StoredType& p)
            : pointee_(p)
            {
                if( p )
                {
                    p->AddRef();
                }
            }

        void ExplicitlyLeakPointer()
            {
                pointee_ = 0;
            }
        
        
        PointerType operator->() const { return pointee_; }
        
        ReferenceType operator*() const { return *pointee_; }
        
        void Swap(FerrisExSmartPtrStorage& rhs)
            { std::swap(pointee_, rhs.pointee_); }
    
        // Accessors
        friend inline PointerType GetImpl(const FerrisExSmartPtrStorage& sp)
            { return sp.pointee_; }

        friend inline PointerType GetImplX(const FerrisExSmartPtrStorage& sp)
            { return sp.pointee_; }
        
        friend inline const StoredType& GetImplRef(const FerrisExSmartPtrStorage& sp)
            { return sp.pointee_; }

        friend inline StoredType& GetImplRef(FerrisExSmartPtrStorage& sp)
            { return sp.pointee_; }

    protected:
        // Destroys the data stored
        // (Destruction might be taken over by the OwnershipPolicy)
        void Destroy()
            { delete pointee_; }
        
        // Default value to initialize the pointer
        static StoredType Default()
            { return 0; }
    
    private:
        // Data
        StoredType pointee_;
    };


    /**
     * An FerrisExSmartPtrStorage for array[] of items. Uses delete[];
     */
    template <class T>
    class FerrisExArraySmartPtrStorage
    {
    public:
        typedef T* StoredType;    // the type of the pointee_ object
        typedef T* InitPointerType;
        typedef T* PointerType;   // type returned by operator->
        typedef T& ReferenceType; // type returned by operator*
        
        FerrisExArraySmartPtrStorage() : pointee_(Default()) 
            {}

        // The storage policy doesn't initialize the stored pointer 
        //     which will be initialized by the OwnershipPolicy's Clone fn
        FerrisExArraySmartPtrStorage(const FerrisExArraySmartPtrStorage&)
            {}

        template <class U>
        FerrisExArraySmartPtrStorage(const FerrisExArraySmartPtrStorage<U>&) 
            {}
        
        FerrisExArraySmartPtrStorage(const StoredType& p)
            : pointee_(p)
            {
//                this->Clone( *p );
            }
        
        PointerType operator->() const { return pointee_; }
        
        ReferenceType operator*() const { return *pointee_; }
        
        void Swap(FerrisExArraySmartPtrStorage& rhs)
            { std::swap(pointee_, rhs.pointee_); }
    
        // Accessors
        friend inline PointerType GetImpl(const FerrisExArraySmartPtrStorage& sp)
            { return sp.pointee_; }

        friend inline PointerType GetImplX(const FerrisExArraySmartPtrStorage& sp)
            { return sp.pointee_; }
        
        friend inline const StoredType& GetImplRef(const FerrisExArraySmartPtrStorage& sp)
            { return sp.pointee_; }

        friend inline StoredType& GetImplRef(FerrisExArraySmartPtrStorage& sp)
            { return sp.pointee_; }

    protected:
        // Destroys the data stored
        // (Destruction might be taken over by the OwnershipPolicy)
        void Destroy()
            {
//                cerr << "FerrisExArraySmartPtrStorage() delete[] " << endl;
                delete [] pointee_;
            }
        
        // Default value to initialize the pointer
        static StoredType Default()
            { return 0; }
    
    private:
        // Data
        StoredType pointee_;
    };


    class FerrisSmallObjAllocator
        :
        public ::Loki::SmallObjAllocator
    {
    public:
        FerrisSmallObjAllocator( std::size_t maxObjectSize,
                                 std::size_t pageSize = LOKI_DEFAULT_CHUNK_SIZE,
                                 std::size_t objectAlignSize = LOKI_DEFAULT_OBJECT_ALIGNMENT )
            :
            SmallObjAllocator( pageSize, maxObjectSize,  objectAlignSize )
            {
            }
        void * Allocate( std::size_t size )
            {
                bool doThrow = true;
                return SmallObjAllocator::Allocate( size, doThrow );
            }
        

    };
    
};

namespace Ferris
{
    template
    <
        typename T1,
        template <class> class OP1,
        class CP1,
        template <class> class KP1,
        template <class> class SP1
    >
    inline T1* GetImpl( const Loki::SmartPtr<T1, OP1, CP1, KP1, SP1>& s )
    {
        return s.getRawPointer();
    }
    
    template <class T,
              class CONVERTER,
              template <class> class CHECKER>
    inline bool isBound( const Loki::SmartPtr< T,
                         FerrisLoki::FerrisExRefCounted, 
                         CONVERTER,
                         CHECKER, 
                         FerrisLoki::FerrisExSmartPtrStorage > & sp)
    {
        return GetImpl(sp) != 0 ;
    }

    template <class T,
              class CONVERTER,
              template <class> class CHECKER,
              template <class> class SPT>
    inline bool isBound( const Loki::SmartPtr< T,
                         FerrisLoki::FerrisExRefCounted, 
                         CONVERTER,
                         CHECKER, 
                         SPT >& sp)
    {
        return GetImpl(sp) != 0 ;
    }

    template < class HT, class OT >
    HT& Sink( HT& handle, const OT& obj )
    {
        HT th( obj );
        handle = th;
        return handle;
    }

    template <class To, class From>
    inline To Upcast( To& dst, From& src )
    {
        dst = GetImpl( src );
    // Solved by using a new storage policy
//        dst->AddRef();
        return dst;
    }

    template <class To, class From>
    inline To Upcast( To& dst, From* src )
    {
        dst = src;
    // Solved by using a new storage policy
//      dst->AddRef();
        return dst;
    }
    
    template <class T>
    void* toVoid( T smptr )
    {
        return (void*)(GetImpl(smptr));
    }

    template <class T>
    void* toVoid( T* obj )
    {
        return (void*)(obj);
    }

#define FERRIS_ARRAY_SMARTPTR(ClassNameT) ::Loki::SmartPtr< \
 ClassNameT, \
 ::Loki::RefCounted, \
 ::Loki::AllowConversion, \
 ::Loki::AssertCheck, \
 ::Loki::ArrayStorage >
   
};
#endif


