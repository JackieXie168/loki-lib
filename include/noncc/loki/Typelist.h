////////////////////////////////
// Generated header: Typelist.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "../../loki/Typelist.h"
#else
#	if (__INTEL_COMPILER)
#		include "../../loki/Typelist.h"
#	elif (__MWERKS__)
#		include "../../loki/Typelist.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "../Borland/Typelist.h"
#	elif (_MSC_VER >= 1301)
#		include "../../loki/Typelist.h"
#	elif (_MSC_VER >= 1300)
#		include "../MSVC/1300/Typelist.h"
#	elif (_MSC_VER >= 1200)
#		include "../MSVC/1200/Typelist.h"
#	else
#		include "../../loki/Typelist.h"
#	endif
#endif
