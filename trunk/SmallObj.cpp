/////////////////////////////////
//Generated header: SmallObj.cpp
//Forwards to the appropriate code
// that works on the detected compiler
//Generated on Thu Aug 22 21:04:30 2002


#ifdef LOKI_USE_REFERENCE
#	include ".\Reference\SmallObj.cpp"
#else
#	if (_MSC_VER >= 1300)
#		include ".\MSVC\1300\SmallObj.cpp"
#	elif (_MSC_VER >= 1200)
#		include ".\MSVC\1200\SmallObj.cpp"
#	elif (__BORLANDC__)
#		include ".\Borland\SmallObj.cpp"
#	elif (__MWERKS__)
#		include ".\Reference\SmallObj.cpp"
#	elif ( (__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 95)) )
#		include ".\Reference\SmallObj.cpp"
#	else
		//Define LOKI_USE_REFERENCE and get back to us on the results
#		error Compiler not tested with Loki, #define LOKI_USE_REFERENCE
#	endif
#endif
