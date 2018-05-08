#ifndef __TINY_CORE__LOCK__MUTEX__H__
#define __TINY_CORE__LOCK__MUTEX__H__


/**
 *
 *  作者: hm
 *
 *  说明: mutex
 *
 */


#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace lock
	{
		using SystemMutex = std::mutex;

		class NullMutex
		{
		public:
			void lock()
			{

			}

			void unlock()
			{

			}

			bool try_lock()
			{
				return true;
			}
		};
	}
}


#endif // __TINY_CORE__LOCK__MUTEX__H__
