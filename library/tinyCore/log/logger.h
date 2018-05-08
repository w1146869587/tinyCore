#ifndef __TINY_CORE__LOG__LOGGER__H__
#define __TINY_CORE__LOG__LOGGER__H__


/**
 *
 *  作者: hm
 *
 *  说明: 日志
 *
 */


#include <tinyCore/log/sink.h>
#include <tinyCore/log/formatter.h>
#include <tinyCore/container/queue.h>


namespace tinyCore
{
	namespace log
	{
		class ILogger
		{
			using SinkPtr = std::shared_ptr<ISink>;
			using SinkVector = std::vector<SinkPtr>;
			using FormatterPtr = std::shared_ptr<LogFormatter>;
			using FlushInterval = std::chrono::milliseconds;
			using SinksInitList = std::initializer_list<SinkPtr>;

		public:
			ILogger() = default;

			explicit ILogger(std::string name) : _name(std::move(name)),
												 _formatter(std::make_shared<LogFormatter>("%+"))
			{

			}

			explicit ILogger(std::string name, FormatterPtr formatter) : _name(std::move(name)),
																		 _formatter(std::move(formatter))
			{

			}

			explicit ILogger(std::string name, std::string formatter) :
					_name(std::move(name)),
					_formatter(std::make_shared<LogFormatter>(std::move(formatter)))
			{

			}

			virtual ~ILogger() = default;

			const std::string & Name() const
			{
				return _name;
			}

			const SinkVector & Sinks() const
			{
				return _managerSink.Sinks();
			}

			TINY_LOG_TYPE Type() const
			{
				return _type.load(std::memory_order_relaxed);
			}

			TINY_LOG_LEVEL Level() const
			{
				return _level.load(std::memory_order_relaxed);
			}

			TINY_LOG_LEVEL AutoFLushLevel() const
			{
				return _autoFlushLevel.load(std::memory_order_relaxed);
			}

			TINY_LOG_FULL_POLICY Policy() const
			{
				return _fullPolicy.load(std::memory_order_relaxed);
			}

			virtual void Wait()
			{

			}

			virtual void Flush()
			{

			}

			void SetType(TINY_LOG_TYPE type)
			{
				_type.store(type);
			}

			void SetLevel(TINY_LOG_LEVEL level)
			{
				_level.store(level);
			}

			void SetAutoFlushLevel(TINY_LOG_LEVEL level)
			{
				_autoFlushLevel.store(level);
			}

			void SetFullPolicy(TINY_LOG_FULL_POLICY policy)
			{
				_fullPolicy.store(policy);
			}

			void SetFormatter(FormatterPtr formatter)
			{
				_formatter = std::move(formatter);
			}

			void SetFormatter(const std::string & pattern)
			{
				_formatter = std::make_shared<LogFormatter>(pattern);
			}

			void SetFlushInterval(const FlushInterval & interval)
			{
				_flushInterval = interval;
			}

			void AddSink(const SinkPtr & sink)
			{
				_managerSink.Add(sink);
			}

			void AddSink(const SinksInitList & sinkList)
			{
				_managerSink.Add(sinkList);
			}

			template<class It>
			void AddSink(const It & begin, const It & end)
			{
				_managerSink.Add(begin, end);
			}

			void RemoveSink()
			{
				_managerSink.Remove();
			}

			void RemoveSink(const SinkPtr & sink)
			{
				_managerSink.Remove(sink);
			}

			void Trace(const char * msg)
			{
				Log(TINY_LOG_LEVEL_TRACE, msg);
			}

			void Debug(const char * msg)
			{
				Log(TINY_LOG_LEVEL_DEBUG, msg);
			}

			void Info(const char * msg)
			{
				Log(TINY_LOG_LEVEL_INFO, msg);
			}

			void Warning(const char * msg)
			{
				Log(TINY_LOG_LEVEL_WARNING, msg);
			}

			void Error(const char * msg)
			{
				Log(TINY_LOG_LEVEL_ERROR, msg);
			}

			void Critical(const char * msg)
			{
				Log(TINY_LOG_LEVEL_CRITICAL, msg);
			}

			void Fatal(const char * msg)
			{
				Log(TINY_LOG_LEVEL_FATAL, msg);
			}

			template<typename TypeT>
			void Trace(const TypeT & msg)
			{
				Log(TINY_LOG_LEVEL_TRACE, msg);
			}

			template<typename TypeT>
			void Debug(const TypeT & msg)
			{
				Log(TINY_LOG_LEVEL_DEBUG, msg);
			}

			template<typename TypeT>
			void Info(const TypeT & msg)
			{
				Log(TINY_LOG_LEVEL_INFO, msg);
			}

			template<typename TypeT>
			void Warning(const TypeT & msg)
			{
				Log(TINY_LOG_LEVEL_WARNING, msg);
			}

			template<typename TypeT>
			void Error(const TypeT & msg)
			{
				Log(TINY_LOG_LEVEL_ERROR, msg);
			}

			template<typename TypeT>
			void Critical(const TypeT & msg)
			{
				Log(TINY_LOG_LEVEL_CRITICAL, msg);
			}

			template<typename TypeT>
			void Fatal(const TypeT & msg)
			{
				Log(TINY_LOG_LEVEL_FATAL, msg);
			}

			template<typename Arg1, typename... Args>
			void Trace(const char * fmt, const Arg1 & arg1, const Args &... args)
			{
				Log(TINY_LOG_LEVEL_TRACE, fmt, arg1, args...);
			}

			template<typename Arg1, typename... Args>
			void Debug(const char * fmt, const Arg1 & arg1, const Args &... args)
			{
				Log(TINY_LOG_LEVEL_DEBUG, fmt, arg1, args...);
			}

			template<typename Arg1, typename... Args>
			void Info(const char * fmt, const Arg1 & arg1, const Args &... args)
			{
				Log(TINY_LOG_LEVEL_INFO, fmt, arg1, args...);
			}

			template<typename Arg1, typename... Args>
			void Warning(const char * fmt, const Arg1 & arg1, const Args &... args)
			{
				Log(TINY_LOG_LEVEL_WARNING, fmt, arg1, args...);
			}

			template<typename Arg1, typename... Args>
			void Error(const char * fmt, const Arg1 & arg1, const Args &... args)
			{
				Log(TINY_LOG_LEVEL_ERROR, fmt, arg1, args...);
			}

			template<typename Arg1, typename... Args>
			void Critical(const char * fmt, const Arg1 & arg1, const Args &... args)
			{
				Log(TINY_LOG_LEVEL_CRITICAL, fmt, arg1, args...);
			}

			template<typename Arg1, typename... Args>
			void Fatal(const char * fmt, const Arg1 & arg1, const Args &... args)
			{
				Log(TINY_LOG_LEVEL_FATAL, fmt, arg1, args...);
			}

		protected:
			virtual void Log(const LogMessage & logMsg)
			{

			}

			void Log(TINY_LOG_LEVEL level, const char * msg)
			{
				TINY_ASSERT(msg, "msg is nullptr")
				TINY_ASSERT(_formatter, "_formatter is nullptr")

				if (!CheckLevel(level))
				{
					return;
				}

				LogMessage logMsg(_name, level);

				logMsg.msg << msg;

				logMsg.messageID = _messageID.fetch_add(1, std::memory_order_relaxed);

				_formatter->Format(logMsg);

				Log(logMsg);
			}

			template<typename TypeT>
			void Log(TINY_LOG_LEVEL level, const TypeT & msg)
			{
				TINY_ASSERT(_formatter, "_formatter is nullptr")

				if (!CheckLevel(level))
				{
					return;
				}

				LogMessage logMsg(_name, level);

				logMsg.msg << msg;

				logMsg.messageID = _messageID.fetch_add(1, std::memory_order_relaxed);

				_formatter->Format(logMsg);

				Log(logMsg);
			}

			template<typename... Args>
			void Log(TINY_LOG_LEVEL level, const char * fmt, const Args &... args)
			{
				TINY_ASSERT(fmt, "fmt is nullptr")
				TINY_ASSERT(_formatter, "_formatter is nullptr")

				if (!CheckLevel(level))
				{
					return;
				}

				LogMessage logMsg(_name, level);

				logMsg.msg.write(fmt, args...);

				logMsg.messageID = _messageID.fetch_add(1, std::memory_order_relaxed);

				_formatter->Format(logMsg);

				Log(logMsg);
			}

			bool CheckLevel(TINY_LOG_LEVEL level) const
			{
				return level >= _level.load(std::memory_order_relaxed);
			}

			bool CheckAutoFLushLevel(TINY_LOG_LEVEL level) const
			{
				return level >= _autoFlushLevel.load(std::memory_order_relaxed);
			}

		protected:
			std::string _name{ TINY_FILE_APPLICATION_NAME().string() };

			FormatterPtr _formatter{ std::make_shared<LogFormatter>("%+") };

			FlushInterval _flushInterval{ std::chrono::milliseconds(100) };

			ManagerSinkSync _managerSink{ };

			std::atomic<size_t> _messageID{ 1 };

			std::atomic<TINY_LOG_TYPE> _type{ TINY_LOG_TYPE_SYNC };

			std::atomic<TINY_LOG_LEVEL> _level{ TINY_LOG_LEVEL_TRACE };
			std::atomic<TINY_LOG_LEVEL> _autoFlushLevel{ TINY_LOG_LEVEL_FATAL };

			std::atomic<TINY_LOG_FULL_POLICY> _fullPolicy{ TINY_LOG_FULL_POLICY_RETRY };
		};

		class SyncLogger : public ILogger
		{
			using FormatterPtr = std::shared_ptr<LogFormatter>;

		public:
			static std::shared_ptr<SyncLogger> Instance()
			{
				static auto instance = std::make_shared<SyncLogger>();

				return instance;
			}

			SyncLogger() : ILogger()
			{
				SetType(TINY_LOG_TYPE_SYNC);
			}

			explicit SyncLogger(std::string name) : ILogger(std::move(name))
			{
				SetType(TINY_LOG_TYPE_SYNC);
			}

			explicit SyncLogger(std::string name, FormatterPtr formatter) : ILogger(std::move(name), std::move(formatter))
			{
				SetType(TINY_LOG_TYPE_SYNC);
			}

			explicit SyncLogger(std::string name, std::string formatter) : ILogger(std::move(name), std::move(formatter))
			{
				SetType(TINY_LOG_TYPE_SYNC);
			}

			~SyncLogger() override
			{
				Flush();
			}

			void Wait() override
			{

			}

			void Flush() override
			{
				_managerSink.Flush();
			}

		protected:
			void Log(const LogMessage & logMsg) override
			{
				static SystemClockTimesPoint lastFlush = TINY_TIME_POINT();

				_managerSink.Write(logMsg);

				if (CheckAutoFLushLevel(logMsg.level))
				{
					isFlush = true;
				}

				FlushLog(lastFlush);
			}

			void FlushLog(SystemClockTimesPoint & last, const SystemClockTimesPoint & now = TINY_TIME_POINT())
			{
				if (isFlush || now - last > _flushInterval)
				{
					last = now;

					isFlush = false;

					_managerSink.Flush();
				}
			}

		protected:
			bool isFlush{ false };
		};

		class AsyncLogger : public ILogger
		{
			using LoggerQueue = container::BoundedQueue<LogMessage>;
			using FormatterPtr = std::shared_ptr<LogFormatter>;

		public:
			static std::shared_ptr<AsyncLogger> Instance()
			{
				static auto instance = std::make_shared<AsyncLogger>();

				return instance;
			}

			AsyncLogger() : ILogger()
			{
				SetType(TINY_LOG_TYPE_ASYNC);
				SetFullPolicy(TINY_LOG_FULL_POLICY_RETRY);

				_thread = std::thread(&AsyncLogger::ThreadProcess, this);
			}

			explicit AsyncLogger(std::string name) : ILogger(std::move(name))
			{
				SetType(TINY_LOG_TYPE_ASYNC);
				SetFullPolicy(TINY_LOG_FULL_POLICY_RETRY);

				_thread = std::thread(&AsyncLogger::ThreadProcess, this);
			}

			explicit AsyncLogger(std::string name, FormatterPtr formatter) : ILogger(std::move(name), std::move(formatter))
			{
				SetType(TINY_LOG_TYPE_ASYNC);
				SetFullPolicy(TINY_LOG_FULL_POLICY_RETRY);

				_thread = std::thread(&AsyncLogger::ThreadProcess, this);
			}

			explicit AsyncLogger(std::string name, std::string formatter) : ILogger(std::move(name), std::move(formatter))
			{
				SetType(TINY_LOG_TYPE_ASYNC);
				SetFullPolicy(TINY_LOG_FULL_POLICY_RETRY);

				_thread = std::thread(&AsyncLogger::ThreadProcess, this);
			}

			~AsyncLogger() override
			{
				try
				{
					Log(LogMessage(_name, TINY_LOG_STATUS_TERMINATE));

					_thread.join();
				}
				catch (...)
				{

				}
			}

			void Wait() override
			{
				SystemClockTimesPoint last = TINY_TIME_POINT();

				while (!_queue.Empty())
				{
					SleepOrYield(last);
				}
			}

			void Flush() override
			{
				Log(LogMessage(_name, TINY_LOG_STATUS_FLUSH));
			}

		protected:
			void ThreadProcess()
			{
				SystemClockTimesPoint lastRead  = TINY_TIME_POINT();
				SystemClockTimesPoint lastFlush = TINY_TIME_POINT();

				while (true)
				{
					if (!ProcessNextLog(lastRead, lastFlush))
					{
						break;
					}
				}
			}

			void Log(const LogMessage & logMsg) override
			{
				PushMessage(logMsg);
			}

			void PushMessage(const LogMessage & logMsg)
			{
				if (!_queue.Write(logMsg) && _fullPolicy == TINY_LOG_FULL_POLICY_RETRY)
				{
					SystemClockTimesPoint last = TINY_TIME_POINT();

					do
					{
						SleepOrYield(last);
					} while(!_queue.Write(logMsg));
				}
			}

			bool ProcessNextLog(SystemClockTimesPoint & lastRead, SystemClockTimesPoint & lastFlush)
			{
				LogMessage logMsg;

				if (_queue.Read(logMsg))
				{
					lastRead = TINY_TIME_POINT();

					if (logMsg.status == TINY_LOG_STATUS_WRITE)
					{
						_managerSink.Write(logMsg);

						if (CheckAutoFLushLevel(logMsg.level))
						{
							Flush();
						}
					}
					else if (logMsg.status == TINY_LOG_STATUS_FLUSH)
					{
						_isFlush = true;
					}
					else if (logMsg.status == TINY_LOG_STATUS_TERMINATE)
					{
						_isFlush = true;
						_isTerminate = true;
					}

					return true;
				}

				FlushLog(lastFlush);

				SleepOrYield(lastRead);

				return !_isTerminate;
			}

			void FlushLog(SystemClockTimesPoint & last, const SystemClockTimesPoint & now = TINY_TIME_POINT())
			{
				if (_isFlush || now - last > _flushInterval)
				{
					last = now;

					_isFlush = false;

					_managerSink.Flush();
				}
			}

			void SleepOrYield(SystemClockTimesPoint & last, const SystemClockTimesPoint & now = TINY_TIME_POINT())
			{
				SystemClockDuration since = now - last;

				if (since <= std::chrono::microseconds(50))
				{
					return;
				}
				else if (since <= std::chrono::microseconds(100))
				{
					return std::this_thread::yield();
				}
				else if (since <= std::chrono::milliseconds(200))
				{
					return TINY_SLEEP_MS(20);
				}
				else
				{
					return TINY_SLEEP_MS(100);
				}
			}

		protected:
			bool _isFlush{ false };
			bool _isTerminate{ false };

			LoggerQueue _queue{ 32 * TINY_KB };

			std::thread _thread{ };
		};

		class LoggerHelper
		{
		public:
			template <typename TypeT, typename... Args>
			static void Trace(ILogger * logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Trace(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Debug(ILogger * logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Debug(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Info(ILogger * logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Info(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Warning(ILogger * logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Warning(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Error(ILogger * logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Error(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Critical(ILogger * logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Critical(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Fatal(ILogger * logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Fatal(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////

			template <typename TypeT, typename... Args>
			static void Trace(ILogger & logger, TypeT && log, Args &&... args)
			{
				logger.Trace(std::forward<TypeT>(log), std::forward<Args>(args)...);
			}

			template <typename TypeT, typename... Args>
			static void Debug(ILogger & logger, TypeT && log, Args &&... args)
			{
				logger.Debug(std::forward<TypeT>(log), std::forward<Args>(args)...);
			}

			template <typename TypeT, typename... Args>
			static void Info(ILogger & logger, TypeT && log, Args &&... args)
			{
				logger.Info(std::forward<TypeT>(log), std::forward<Args>(args)...);
			}

			template <typename TypeT, typename... Args>
			static void Warning(ILogger & logger, TypeT && log, Args &&... args)
			{
				logger.Warning(std::forward<TypeT>(log), std::forward<Args>(args)...);
			}

			template <typename TypeT, typename... Args>
			static void Error(ILogger & logger, TypeT && log, Args &&... args)
			{
				logger.Error(std::forward<TypeT>(log), std::forward<Args>(args)...);
			}

			template <typename TypeT, typename... Args>
			static void Critical(ILogger & logger, TypeT && log, Args &&... args)
			{
				logger.Critical(std::forward<TypeT>(log), std::forward<Args>(args)...);
			}

			template <typename TypeT, typename... Args>
			static void Fatal(ILogger & logger, TypeT && log, Args &&... args)
			{
				logger.Fatal(std::forward<TypeT>(log), std::forward<Args>(args)...);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////

			template <typename TypeT, typename... Args>
			static void Trace(const std::shared_ptr<ILogger> & logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Trace(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Debug(const std::shared_ptr<ILogger> & logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Debug(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Info(const std::shared_ptr<ILogger> & logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Info(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Warning(const std::shared_ptr<ILogger> & logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Warning(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Error(const std::shared_ptr<ILogger> & logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Error(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Critical(const std::shared_ptr<ILogger> & logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Critical(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}

			template <typename TypeT, typename... Args>
			static void Fatal(const std::shared_ptr<ILogger> & logger, TypeT && log, Args &&... args)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				if (logger)
				{
					logger->Fatal(std::forward<TypeT>(log), std::forward<Args>(args)...);
				}
			}
		};
	}
}


#define sSyncLogger tinyCore::log::SyncLogger::Instance()
#define sAsyncLogger tinyCore::log::AsyncLogger::Instance()


#define TINY_LOG_INFO(logger, fmt, ...)				tinyCore::log::LoggerHelper::Info(logger, fmt, ##__VA_ARGS__);
#define TINY_LOG_TRACE(logger, fmt, ...)			tinyCore::log::LoggerHelper::Trace(logger, fmt, ##__VA_ARGS__);
#define TINY_LOG_DEBUG(logger, fmt, ...)			tinyCore::log::LoggerHelper::Debug(logger, fmt, ##__VA_ARGS__);
#define TINY_LOG_ERROR(logger, fmt, ...)			tinyCore::log::LoggerHelper::Error(logger, fmt, ##__VA_ARGS__);
#define TINY_LOG_FATAL(logger, fmt, ...)			tinyCore::log::LoggerHelper::Fatal(logger, fmt, ##__VA_ARGS__);
#define TINY_LOG_WARNING(logger, fmt, ...)			tinyCore::log::LoggerHelper::Warning(logger, fmt, ##__VA_ARGS__);
#define TINY_LOG_CRITICAL(logger, fmt, ...)			tinyCore::log::LoggerHelper::Critical(logger, fmt, ##__VA_ARGS__);


#define TINY_LOG_SYNC_INFO(fmt, ...)				tinyCore::log::LoggerHelper::Info(sSyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_SYNC_TRACE(fmt, ...)				tinyCore::log::LoggerHelper::Trace(sSyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_SYNC_DEBUG(fmt, ...)				tinyCore::log::LoggerHelper::Debug(sSyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_SYNC_ERROR(fmt, ...)				tinyCore::log::LoggerHelper::Error(sSyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_SYNC_FATAL(fmt, ...)				tinyCore::log::LoggerHelper::Fatal(sSyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_SYNC_WARNING(fmt, ...)				tinyCore::log::LoggerHelper::Warning(sSyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_SYNC_CRITICAL(fmt, ...)			tinyCore::log::LoggerHelper::Critical(sSyncLogger, fmt, ##__VA_ARGS__);


#define TINY_LOG_ASYNC_INFO(fmt, ...)				tinyCore::log::LoggerHelper::Info(sAsyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_ASYNC_TRACE(fmt, ...)				tinyCore::log::LoggerHelper::Trace(sAsyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_ASYNC_DEBUG(fmt, ...)				tinyCore::log::LoggerHelper::Debug(sAsyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_ASYNC_ERROR(fmt, ...)				tinyCore::log::LoggerHelper::Error(sAsyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_ASYNC_FATAL(fmt, ...)				tinyCore::log::LoggerHelper::Fatal(sAsyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_ASYNC_WARNING(fmt, ...)			tinyCore::log::LoggerHelper::Warning(sAsyncLogger, fmt, ##__VA_ARGS__);
#define TINY_LOG_ASYNC_CRITICAL(fmt, ...)			tinyCore::log::LoggerHelper::Critical(sAsyncLogger, fmt, ##__VA_ARGS__);


#define TINY_LOG_INFO_IF(cond, logger, fmt, ...)		if ( (cond)) { tinyCore::log::LoggerHelper::Info(logger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_TRACE_IF(cond, logger, fmt, ...)		if ( (cond)) { tinyCore::log::LoggerHelper::Trace(logger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_DEBUG_IF(cond, logger, fmt, ...)		if ( (cond)) { tinyCore::log::LoggerHelper::Debug(logger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_ERROR_IF(cond, logger, fmt, ...)		if ( (cond)) { tinyCore::log::LoggerHelper::Error(logger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_FATAL_IF(cond, logger, fmt, ...)		if ( (cond)) { tinyCore::log::LoggerHelper::Fatal(logger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_WARNING_IF(cond, logger, fmt, ...)		if ( (cond)) { tinyCore::log::LoggerHelper::Warning(logger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_CRITICAL_IF(cond, logger, fmt, ...)	if ( (cond)) { tinyCore::log::LoggerHelper::Critical(logger, fmt, ##__VA_ARGS__); }


#define TINY_LOG_SYNC_INFO_IF(cond, fmt, ...)			if ( (cond)) { tinyCore::log::LoggerHelper::Info(sSyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_SYNC_TRACE_IF(cond, fmt, ...)			if ( (cond)) { tinyCore::log::LoggerHelper::Trace(sSyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_SYNC_DEBUG_IF(cond, fmt, ...)			if ( (cond)) { tinyCore::log::LoggerHelper::Debug(sSyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_SYNC_ERROR_IF(cond, fmt, ...)			if ( (cond)) { tinyCore::log::LoggerHelper::Error(sSyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_SYNC_FATAL_IF(cond, fmt, ...)			if ( (cond)) { tinyCore::log::LoggerHelper::Fatal(sSyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_SYNC_WARNING_IF(cond, fmt, ...)		if ( (cond)) { tinyCore::log::LoggerHelper::Warning(sSyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_SYNC_CRITICAL_IF(cond, fmt, ...)		if ( (cond)) { tinyCore::log::LoggerHelper::Critical(sSyncLogger, fmt, ##__VA_ARGS__); }


#define TINY_LOG_ASYNC_INFO_IF(cond, fmt, ...)			if ( (cond)) { tinyCore::log::LoggerHelper::Info(sAsyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_ASYNC_TRACE_IF(cond, fmt, ...)			if ( (cond)) { tinyCore::log::LoggerHelper::Trace(sAsyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_ASYNC_DEBUG_IF(cond, fmt, ...)			if ( (cond)) { tinyCore::log::LoggerHelper::Debug(sAsyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_ASYNC_ERROR_IF(cond, fmt, ...)			if ( (cond)) { tinyCore::log::LoggerHelper::Error(sAsyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_ASYNC_FATAL_IF(cond, fmt, ...)			if ( (cond)) { tinyCore::log::LoggerHelper::Fatal(sAsyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_ASYNC_WARNING_IF(cond, fmt, ...)		if ( (cond)) { tinyCore::log::LoggerHelper::Warning(sAsyncLogger, fmt, ##__VA_ARGS__); }
#define TINY_LOG_ASYNC_CRITICAL_IF(cond, fmt, ...)		if ( (cond)) { tinyCore::log::LoggerHelper::Critical(sAsyncLogger, fmt, ##__VA_ARGS__); }


#endif // __TINY_CORE__LOG__LOGGER__H__
