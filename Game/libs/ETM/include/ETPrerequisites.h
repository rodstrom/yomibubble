#ifndef __ETPREREQUISITES_H__
#define __ETPREREQUISITES_H__

#include <OgrePlatform.h>

#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32 )
#   ifdef ET_MANAGER_EXPORTS
#       define _ETManagerExport __declspec(dllexport)
#   else
#       if defined( __MINGW32__ )
#           define _ETManagerExport
#       else
#    		define _ETManagerExport __declspec(dllimport)
#       endif
#   endif
#elif defined ( OGRE_GCC_VISIBILITY )
#    define _ETManagerExport  __attribute__ ((visibility("default")))
#else
#   define _ETManagerExport
#endif

#endif
