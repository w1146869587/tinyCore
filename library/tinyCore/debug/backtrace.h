#ifndef __TINY_CORE__DEBUG__BACKTRACE__H__
#define __TINY_CORE__DEBUG__BACKTRACE__H__


/**
 *
 *  作者: hm
 *
 *  说明: 回溯堆栈
 *
 */


#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace debug
	{
		class Backtrace
		{
		public:
			static void Trace(int32_t signalNo)
			{
				void * array[TINY_KB];

				int32_t size = backtrace(array, TINY_KB);

				char ** stackString = backtrace_symbols(array, size);

				std::cerr << "[Trace] Signal " << signalNo << ", Obtained " << size << " stack frames" << std::endl;

				if (stackString)
				{
					for (int32_t i = 0; i < size; ++i)
					{
						std::cerr << "\t#" << i << " " << stackString[i] << std::endl;
					}

					free(stackString);
				}

				exit(0);
			}

			static void Trace(int32_t signalNo, siginfo_t * info, void * secret)
			{
				assert(info && secret);

				void * array[TINY_KB];

				int32_t size = backtrace(array, TINY_KB);

				if (GetMcontextEip((ucontext_t*)secret) != nullptr)
				{
					array[1] = GetMcontextEip((ucontext_t*)secret);
				}

				char ** stackString = backtrace_symbols(array, size);

				std::cerr << "[Trace] Signal " << signalNo << ", Obtained " << size << " stack frames" << std::endl;

				if (stackString)
				{
					for (int32_t i = 0; i < size; ++i)
					{
						std::cerr << "\t#" << i << " " << stackString[i] << std::endl;
					}

					free(stackString);
				}

				exit(0);
			}

		protected:
			static void * GetMcontextEip(ucontext_t * uc)
			{
				assert(uc);

			#if defined(__APPLE__) && !defined(MAC_OS_X_VERSION_10_6)

				#if defined(__x86_64__)

				return (void*) uc->uc_mcontext->__ss.__rip;

				#elif defined(__i386__)

				return (void*) uc->uc_mcontext->__ss.__eip;

				#else

				return (void*) uc->uc_mcontext->__ss.__srr0;

				#endif

			#elif defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)

				#if defined(_STRUCT_X86_THREAD_STATE64) && !defined(__i386__)

				return (void*) uc->uc_mcontext->__ss.__rip;

				#else

				return (void*) uc->uc_mcontext->__ss.__eip;

				#endif

			#elif defined(__linux__)

				#if defined(__i386__)

				return (void*) uc->uc_mcontext.gregs[14];

				#elif defined(__X86_64__) || defined(__x86_64__)

				return (void*) uc->uc_mcontext.gregs[16];

				#elif defined(__ia64__)

				return (void*) uc->uc_mcontext.sc_ip;

				#endif

			#else

				return nullptr;

			#endif
			}
		};
	}
}


#define TINY_BACK_TRACE(signalNo, ...) tinyCore::debug::Backtrace::Trace(signalNo, ##__VA_ARGS__)


#endif // __TINY_CORE__DEBUG__BACK_TRACE__H__
