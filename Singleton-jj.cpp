// Loki 侯捷調整版
// 檔名 : Singleton-jj.cpp
// 根據 : Loki's Singleton.cpp
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

#include "Singleton-jj.h"
#include <iostream>
using namespace std;

using namespace Loki::Private;

// 以下兩行原本是 extern。編譯不過（該當如此）。侯捷把 extern 拿掉。
TrackerArray Loki::Private::pTrackerArray = 0;
unsigned int Loki::Private::elements = 0;

////////////////////////////////////////////////////////////////////////////////
// function AtExitFn
// Ensures proper destruction of objects with longevity
////////////////////////////////////////////////////////////////////////////////

void Loki::Private::AtExitFn()
{
    // cout << "AtExitFn()" << endl;  // jjhou
    
    assert(elements > 0 && pTrackerArray != 0);
    // Pick the element at the top of the stack
    LifetimeTracker* pTop = pTrackerArray[elements - 1];
    // Remove that object off the stack
    // Don't check errors - realloc with less memory 
    //     can't fail
    pTrackerArray = static_cast<TrackerArray>(std::realloc(
        pTrackerArray, --elements));
    // Destroy the element
    delete pTop;
}
