#ifndef __TINY_CORE__THREAD__C_THREAD__H__
#define __TINY_CORE__THREAD__C_THREAD__H__


/**
 *
 *  作者: hm
 *
 *  说明: 应用线程
 *
 */


#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace thread
	{
		class AppThreadC
		{
			enum class THREAD_STATUS : uint8_t
			{
				UNINITIALIZED,
				START,
				STOP,
			};

		public:
			AppThreadC() = default;

			virtual ~AppThreadC() = default;

			bool IsStop() const
			{
				return _threadStatus == THREAD_STATUS::STOP;
			}

			bool IsStart() const
			{
				return _threadStatus == THREAD_STATUS::START;
			}

			bool CreateThread()
			{
				if (IsStart())
				{
					return false;
				}

				_threadStatus = THREAD_STATUS::START;

				if (pthread_create(&_id, nullptr, ThreadHandle, (void*)this) != 0)
				{
					_threadStatus = THREAD_STATUS::UNINITIALIZED;

					TINY_THROW_EXCEPTION(debug::ThreadError, TINY_STR_TO_STRING("pthread_create() : ", strerror(errno)));

					return false;
				}

				return true;
			}

			void CloseThread()
			{
				if (IsStart())
				{
					_threadStatus = THREAD_STATUS::STOP;

					pthread_join(_id, nullptr);
				}
			}

		protected:
			virtual void ThreadProcess() = 0;

			static void * ThreadHandle(void * handler)
			{
				if (handler == nullptr)
				{
					TINY_THROW_EXCEPTION(debug::ThreadError, "a null thread owner point")

					return nullptr;
				}

				auto * handlerPtr = (AppThreadC *)handler;

				handlerPtr->ThreadProcess();

				return handlerPtr;
			}

		protected:
			pthread_t _id{ 0 };

			THREAD_STATUS _threadStatus{ THREAD_STATUS::UNINITIALIZED };
		};
	}
}


#endif // __TINY_CORE__THREAD__C_THREAD__H__
