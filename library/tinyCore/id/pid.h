#ifndef __TINY_CORE__ID__PID__H__
#define __TINY_CORE__ID__PID__H__


/**
 *
 *  作者: hm
 *
 *  说明: 获取pid
 *
 */


#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace id
	{
		class PID
		{
		public:
			static uint64_t Create()
			{
			#if TINY_PLATFORM == TINY_PLATFORM_WINDOWS

				return static_cast<size_t>(::GetCurrentProcessId());

			#else

				return static_cast<uint64_t>(::getpid());

			#endif
			}
		};
	}
}


#define TINY_ID_PID() tinyCore::id::PID::Create()


#endif // __TINY_CORE__ID__PID__H__
