#ifndef __TINY_CORE__COMMON__PLATFORM__H__
#define __TINY_CORE__COMMON__PLATFORM__H__


#include <tinyCore/common/macro.h>


#if defined( WIN32 ) || defined( __WIN32 ) || defined( __WIN32__ ) || \
	defined( WIN64 ) || defined( __WIN64 ) || defined( __WIN64__ )
#
#  define TINY_PLATFORM TINY_PLATFORM_WINDOWS
#
#elif defined( APPLE ) || defined( __APPLE ) || defined( __APPLE__ )
#
#  define TINY_PLATFORM TINY_PLATFORM_APPLE
#
#elif defined( INTEL_COMPILER ) || defined( __INTEL_COMPILER ) || defined( __INTEL_COMPILER__ )
#
#  define TINY_PLATFORM TINY_PLATFORM_INTEL
#
#else
#
#  define TINY_PLATFORM TINY_PLATFORM_UNIX
#
#endif


#endif // __TINY_CORE__COMMON__PLATFORM__H__
