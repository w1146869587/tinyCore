#ifndef __TINY_CORE__DEBUG__TRACE__H__
#define __TINY_CORE__DEBUG__TRACE__H__


/**
 *
 *  作者: hm
 *
 *  说明: 调试跟踪
 *
 */


#include <tinyCore/debug/exception.h>
#include <tinyCore/utilities/string.h>
#include <tinyCore/utilities/digital.h>


namespace tinyCore
{
	namespace debug
	{
		class TINY_API Trace
		{
		public:
			static void AssertHandler(const char * file, const int32_t line, const char * func, const char * cond)
			{
				ExceptionHelper::ThrowException<AssertionError>(file, line, func, cond);
			}

			template <typename... Args>
			static void AssertHandler(const char * file, const int32_t line, const char * func, const char * cond, const char * format, Args &&... args)
			{
				std::string error = TINY_STR_FORMAT(format, std::forward<Args>(args)...);

				ExceptionHelper::ThrowException<AssertionError>(file, line, func, cond, std::move(error));
			}

			template <typename... Args>
			static void FatalHandler(char const * file, const int32_t line, char const * func, const char * format, Args &&... args)
			{
				std::string error = TINY_STR_FORMAT(format, std::forward<Args>(args)...);

				ExceptionHelper::ThrowException<SystemExit>(file, line, func, std::move(error));
			}

			template <typename... Args>
			static void WarningHandler(char const * file, const int32_t line, char const * func, char const * format, Args &&... args)
			{
				std::string error = TINY_STR_FORMAT(format, std::forward<Args>(args)...);

				fprintf(stderr, "\n%s:%d %s: Warning (%s)\n\n", file, line, func, error.c_str());

				fflush(stderr);
			}
		};
	}
}


#define TINY_FATAL(...)		tinyCore::debug::Trace::FatalHandler(__FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__);
#define TINY_WARNING(...)	tinyCore::debug::Trace::WarningHandler(__FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__);

#define TINY_FATAL_IF(cond, ...)		if ( (cond)) { tinyCore::debug::Trace::FatalHandler(__FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__); }
#define TINY_WARNING_IF(cond, ...)		if ( (cond)) { tinyCore::debug::Trace::WarningHandler(__FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__); }

#define TINY_FATAL_ELSE(cond, ...)		if (!(cond)) { tinyCore::debug::Trace::FatalHandler(__FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__); }
#define TINY_WARNING_ELSE(cond, ...)	if (!(cond)) { tinyCore::debug::Trace::WarningHandler(__FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__); }


#ifdef DEBUG
#
#define TINY_ASSERT(cond, ...) if (!(cond)) { tinyCore::debug::Trace::AssertHandler(__FILE__, __LINE__, __PRETTY_FUNCTION__, #cond, ##__VA_ARGS__); }
#
#else
#
#define TINY_ASSERT(cond, ...)
#
#endif


#endif // __TINY_CORE__DEBUG__TRACE__H__
