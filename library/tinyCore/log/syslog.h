#ifndef __TINY_CORE__LOG__SYSLOG__H__
#define __TINY_CORE__LOG__SYSLOG__H__


/**
 *
 *  作者: hm
 *
 *  说明: linux系统日志
 *
 */


#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace log
	{
		#if TINY_PLATFORM == TINY_PLATFORM_UNIX

		#define SYSLOG(fmt...) syslog(LOG_USER | LOG_AUTH | LOG_INFO, fmt);

		#endif
	}
}


#endif // __TINY_CORE__LOG__SYSLOG__H__
