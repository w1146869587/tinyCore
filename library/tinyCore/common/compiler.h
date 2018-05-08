#ifndef __TINY_CORE__COMMON__COMPILER__H__
#define __TINY_CORE__COMMON__COMPILER__H__


#include <tinyCore/common/macro.h>


#if defined( _MSC_VER )
#
#  define TINY_COMPILER TINY_COMPILER_MICROSOFT
#
#elif defined( __BORLANDC__ )
#
#  define TINY_COMPILER TINY_COMPILER_BORLAND
#
#elif defined( __INTEL_COMPILER )
#
#  define TINY_COMPILER TINY_COMPILER_INTEL
#
#elif defined( __GNUC__ )
#
#  define TINY_COMPILER TINY_COMPILER_GNU
#
#  define TINY_GCC_VERSION ( TINY_CONVERT_VALUE_STRING(__GNUC__) \
							 TINY_CONVERT_VALUE_STRING(.) \
							 TINY_CONVERT_VALUE_STRING(__GNUC_MINOR__) \
							 TINY_CONVERT_VALUE_STRING(.) \
							 TINY_CONVERT_VALUE_STRING(__GNUC_PATCHLEVEL__) )
#
#else
#
#  error "FATAL ERROR: Unknown compiler."
#
#endif


#endif // __TINY_CORE__COMMON__COMPILER__H__
