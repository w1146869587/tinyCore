#ifndef __TINY_CORE__LOG__DETAIL__H__
#define __TINY_CORE__LOG__DETAIL__H__


/**
 *
 *  作者: hm
 *
 *  说明: 日志细节
 *
 */


#include <tinyCore/id/pid.h>
#include <tinyCore/id/threadID.h>
#include <tinyCore/utilities/time.h>
#include <tinyCore/system/fileSystem.h>


namespace tinyCore
{
	namespace log
	{
		enum class TINY_LOG_TYPE : uint8_t
		{
			SYNC,
			ASYNC,
		};

		enum class TINY_LOG_STATUS : uint8_t
		{
			WRITE,
			FLUSH,
			TERMINATE,
		};

		enum class TINY_LOG_LEVEL : uint8_t
		{
			TRACE,
			DEBUGGER,
			INFO,
			WARNING,
			ERROR,
			CRITICAL,
			FATAL,
		};

		enum class TINY_LOG_FULL_POLICY : uint8_t
		{
			RETRY,
			DISCARD,
		};

		typedef struct LogMessage
		{
			LogMessage() = default;

			explicit LogMessage(TINY_LOG_STATUS logStatus) : status(logStatus)
			{

			}

			explicit LogMessage(std::string logName, TINY_LOG_LEVEL logLevel) : name(std::move(logName)),
																				level(logLevel)
			{

			}

			explicit LogMessage(std::string logName, TINY_LOG_STATUS logStatus) : name(std::move(logName)),
																				  status(logStatus)
			{

			}

			explicit LogMessage(std::string logName, TINY_LOG_LEVEL logLevel, TINY_LOG_STATUS logStatus) : name(std::move(logName)),
																										   level(logLevel),
																										   status(logStatus)
			{

			}

			LogMessage(const LogMessage & rhs) : tm(rhs.tm),
												 threadID(rhs.threadID),
												 messageID(rhs.messageID),
												 name(rhs.name),
												 data(rhs.data),
												 level(rhs.level),
												 status(rhs.status),
												 time(rhs.time)
			{
				msg.clear();
				formatted.clear();

				msg << rhs.msg.c_str();
				formatted << rhs.formatted.c_str();
			}

			LogMessage(LogMessage && rhs) noexcept : tm(rhs.tm),
													 threadID(rhs.threadID),
													 messageID(rhs.messageID),
													 name(std::move(rhs.name)),
													 data(std::move(rhs.data)),
													 level(rhs.level),
													 status(rhs.status),
													 msg(std::move(msg)),
													 formatted(std::move(formatted)),
													 time(rhs.time)
			{

			}

			LogMessage & operator=(const LogMessage & rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}

				tm = rhs.tm;

				threadID = rhs.threadID;
				messageID = rhs.messageID;

				name = rhs.name;
				data = rhs.data;

				level = rhs.level;
				status = rhs.status;

				msg.clear();
				formatted.clear();

				msg << rhs.msg.c_str();
				formatted << rhs.formatted.c_str();

				time = rhs.time;

				return *this;
			}

			LogMessage & operator=(LogMessage && rhs) noexcept
			{
				tm = rhs.tm;

				threadID = rhs.threadID;
				messageID = rhs.messageID;

				name = std::move(rhs.name);
				data = std::move(rhs.data);

				level = rhs.level;
				status = rhs.status;

				msg = std::move(rhs.msg);
				formatted = std::move(rhs.formatted);

				time = rhs.time;

				return *this;
			}

			std::tm tm{ };

			std::size_t threadID{ TINY_ID_THREAD_ID() };
			std::size_t messageID{ 0 };

			std::string name{ };
			std::string data{ };

			TINY_LOG_LEVEL level{ TINY_LOG_LEVEL::TRACE };
			TINY_LOG_STATUS status{ TINY_LOG_STATUS::WRITE };

			fmt::MemoryWriter msg{ };
			fmt::MemoryWriter formatted{ };

			SystemClockTimesPoint time{ TINY_TIME_POINT() };
		}LogMessage;


		#ifndef TINY_LOG_END
		#
		#  if PLATFORM == PLATFORM_WINDOWS
		#
		#    define TINY_LOG_END "\r\n"
		#
		#  else
		#
		#    define TINY_LOG_END "\n"
		#
		#  endif
		#
		#endif


		#define TINY_LOG_TYPE_SYNC				TINY_LOG_TYPE::SYNC
		#define TINY_LOG_TYPE_ASYNC				TINY_LOG_TYPE::ASYNC

		#define TINY_LOG_STATUS_WRITE			TINY_LOG_STATUS::WRITE
		#define TINY_LOG_STATUS_FLUSH			TINY_LOG_STATUS::FLUSH
		#define TINY_LOG_STATUS_TERMINATE		TINY_LOG_STATUS::TERMINATE

		#define TINY_LOG_LEVEL_TRACE			TINY_LOG_LEVEL::TRACE
		#define TINY_LOG_LEVEL_DEBUG			TINY_LOG_LEVEL::DEBUGGER
		#define TINY_LOG_LEVEL_INFO				TINY_LOG_LEVEL::INFO
		#define TINY_LOG_LEVEL_WARNING			TINY_LOG_LEVEL::WARNING
		#define TINY_LOG_LEVEL_ERROR			TINY_LOG_LEVEL::ERROR
		#define TINY_LOG_LEVEL_CRITICAL			TINY_LOG_LEVEL::CRITICAL
		#define TINY_LOG_LEVEL_FATAL			TINY_LOG_LEVEL::FATAL

		#define TINY_LOG_FULL_POLICY_RETRY		TINY_LOG_FULL_POLICY::RETRY
		#define TINY_LOG_FULL_POLICY_DISCARD	TINY_LOG_FULL_POLICY::DISCARD
	}
}


#endif // __TINY_CORE__LOG__DETAIL__H__
