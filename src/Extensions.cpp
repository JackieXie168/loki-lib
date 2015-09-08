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

    $Id: Extensions.cpp,v 1.3 2007/10/23 21:30:49 ben Exp $

*******************************************************************************
*******************************************************************************
******************************************************************************/

#include <Extensions.hh>
#include <iostream>
//#include <execinfo.h>
#include <unistd.h>

namespace FerrisLoki
{


    Handlable::Handlable()
        :
        ref_count(0)
    {
    }
    Handlable::~Handlable()
    {
    }

    Handlable::ref_count_t
    Handlable::AddRef()
    {
        return ++ref_count;
    }

    Handlable::ref_count_t
    Handlable::Release()
    {
//        g_return_val_if_fail( ref_count >= 1 , 1 );
        if( !(ref_count >= 1) )
        {
//             std::cerr << "ASSERT Handlable::Release() ref_count >= 1 failed" << std::endl;
//             const int arraysz = 500;
//             void* array[arraysz];
//             size_t size;

//             size = backtrace( array, arraysz );
//             backtrace_symbols_fd( array, size, STDERR_FILENO );
            return 1;
        }
        
        return --ref_count;
    }

    Handlable::ref_count_t
    Handlable::getReferenceCount()
    {
        return ref_count;
    }

    Handlable::GenericCloseSignal_t&
    Handlable::getGenericCloseSig()
    {
        return GenericCloseSignal;
    }

    void
    Handlable::private_AboutToBeDeleted()
    {
        getGenericCloseSig().emit( this );
    }

 
};
