#ifndef __TINY_CORE__ID__THREAD_ID__H__
#define __TINY_CORE__ID__THREAD_ID__H__


/**
 *
 *  作者: hm
 *
 *  说明: 获取threadID
 *
 */


#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace id
	{
		class ThreadID
		{
		public:
			static uint64_t Create()
			{
				static thread_local const uint64_t tid = GetThreadID();

				return tid;
			}

		protected:
			inline static uint64_t GetThreadID()
			{
				// It exists because the std::this_thread::get_id() is much slower(especially under VS 2013)

			#if TINY_PLATFORM == TINY_PLATFORM_WINDOWS

				return static_cast<uint64_t>(::GetCurrentThreadId());

			#elif TINY_PLATFORM == TINY_PLATFORM_APPLE

				uint64_t tid;

				pthread_threadid_np(nullptr, &tid);

				return tid;

			#elif TINY_PLATFORM == TINY_PLATFORM_INTEL

				long tid;

				thr_self(&tid);

				return static_cast<uint64_t>(tid);

			#elif TINY_PLATFORM == TINY_PLATFORM_UNIX

				return static_cast<uint64_t>(syscall(SYS_gettid));


			#else  // Default to standard C++11 (other Unix)

				return static_cast<uint64_t>(std::hash<std::thread::id>()(std::this_thread::get_id()));

			#endif
			}
		};
	}
}


#define TINY_ID_THREAD_ID() tinyCore::id::ThreadID::Create()


#endif // __TINY_CORE__ID__THREAD_ID__H__
