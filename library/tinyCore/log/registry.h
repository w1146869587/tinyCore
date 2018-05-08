#ifndef __TINY_CORE__LOG__REGISTRY__H__
#define __TINY_CORE__LOG__REGISTRY__H__


/**
 *
 *  作者: hm
 *
 *  说明: 注册日志
 *
 */


#include <tinyCore/log/logger.h>


namespace tinyCore
{
	namespace log
	{
		class RegistryLogger
		{
			using SinkPtr = std::shared_ptr<ISink>;
			using LoggerPtr = std::shared_ptr<ILogger>;
			using LoggerMap = std::map<std::string, LoggerPtr>;
			using SinkVector = std::vector<SinkPtr>;
			using FormatterPtr = std::shared_ptr<LogFormatter>;
			using SinkInitList = std::initializer_list<SinkPtr>;
			using FlushInterval = std::chrono::milliseconds;

		public:
			static void Remove()
			{
				std::lock_guard<std::mutex> lock(_lock);

				_loggers.clear();
			}

			template <typename TypeT>
			static void Remove(const TypeT & name)
			{
				std::lock_guard<std::mutex> lock(_lock);

				_loggers.erase(name);
			}

			static void ApplyCallback(const std::function<void(LoggerPtr)> & callback)
			{
				std::lock_guard<std::mutex> lock(_lock);

				for (auto &iter : _loggers)
				{
					callback(iter.second);
				}
			}

			static void SetLevel(TINY_LOG_LEVEL level)
			{
				std::lock_guard<std::mutex> lock(_lock);

				for (auto &iter : _loggers)
				{
					if (iter.second)
					{
						iter.second->SetLevel(level);
					}
				}
			}

			static void SetAutoFlushLevel(TINY_LOG_LEVEL level)
			{
				std::lock_guard<std::mutex> lock(_lock);

				for (auto &iter : _loggers)
				{
					if (iter.second)
					{
						iter.second->SetAutoFlushLevel(level);
					}
				}
			}

			static void SetFullPolicy(TINY_LOG_FULL_POLICY policy)
			{
				std::lock_guard<std::mutex> lock(_lock);

				for (auto &iter : _loggers)
				{
					if (iter.second)
					{
						iter.second->SetFullPolicy(policy);
					}
				}
			}

			static void SetFormatter(const std::string & formatter)
			{
				std::lock_guard<std::mutex> lock(_lock);

				for (auto &iter : _loggers)
				{
					if (iter.second)
					{
						iter.second->SetFormatter(formatter);
					}
				}
			}

			static void SetFormatter(const FormatterPtr & formatter)
			{
				std::lock_guard<std::mutex> lock(_lock);

				for (auto &iter : _loggers)
				{
					if (iter.second)
					{
						iter.second->SetFormatter(formatter);
					}
				}
			}

			static void SetFlushInterval(const FlushInterval & interval)
			{
				std::lock_guard<std::mutex> lock(_lock);

				for (auto &iter : _loggers)
				{
					if (iter.second)
					{
						iter.second->SetFlushInterval(interval);
					}
				}
			}

			static LoggerPtr Find(const std::string & name = "")
			{
				std::lock_guard<std::mutex> lock(_lock);

				auto found = _loggers.find(GetLoggerName(name));

				return found == _loggers.end() ? nullptr : found->second;
			}

			static LoggerPtr Get(TINY_LOG_TYPE type, const std::string & name = "")
			{
				return GetLoggerByName(type, std::move(GetLoggerName(name)));
			}

			static LoggerPtr GetSync(const std::string & name = "")
			{
				return GetLoggerByName(TINY_LOG_TYPE_SYNC, std::move(GetLoggerName(name)));
			}

			static LoggerPtr GetAsync(const std::string & name = "")
			{
				return GetLoggerByName(TINY_LOG_TYPE_ASYNC, std::move(GetLoggerName(name)));
			}

			static LoggerPtr GetChild(const LoggerPtr & logger, const std::string & suffix)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				return GetLoggerByName(logger->Type(), std::move(JoinLoggerName(logger->Name(), suffix)));
			}

			static LoggerPtr Registry(TINY_LOG_TYPE type, const std::string & name = "")
			{
				return Get(type, name);
			}

			template<typename SinkType, typename... Args>
			static LoggerPtr Registry(TINY_LOG_TYPE type, const std::string & name, Args &&... args)
			{
				return AddSink(Registry(type, name), std::make_shared<SinkType>(std::forward<Args>(args)...));
			}

			static LoggerPtr Registry(TINY_LOG_TYPE type, const std::string & name, const SinkPtr & sink)
			{
				return AddSink(Registry(type, name), sink);
			}

			static LoggerPtr Registry(TINY_LOG_TYPE type, const std::string & name, const SinkInitList & sinkList)
			{
				return AddSink(Registry(type, name), sinkList);
			}

			template<class It>
			static LoggerPtr Registry(TINY_LOG_TYPE type, const std::string & name, const It & begin, const It & end)
			{
				return AddSink(Registry(type, name), begin, end);
			}

			static LoggerPtr RegistryChild(const LoggerPtr & logger, const std::string & suffix)
			{
				return GetChild(logger, suffix);
			}

			template<typename SinkType, typename... Args>
			static LoggerPtr RegistryChild(const LoggerPtr & logger, const std::string & suffix, Args... args)
			{
				return AddSink(RegistryChild(logger, suffix), std::make_shared<SinkType>(std::forward<Args>(args)...));
			}

			static LoggerPtr RegistryChild(const LoggerPtr & logger, const std::string & suffix, const SinkPtr & sink)
			{
				return AddSink(RegistryChild(logger, suffix), sink);
			}

			static LoggerPtr RegistryChild(const LoggerPtr & logger, const std::string & suffix, const SinkInitList & sinkList)
			{
				return AddSink(RegistryChild(logger, suffix), sinkList);
			}

			template<class It>
			static LoggerPtr RegistryChild(const LoggerPtr & logger, const std::string & suffix, const It & begin,
																								 const It & end)
			{
				return AddSink(RegistryChild(logger, suffix), begin, end);
			}

			static LoggerPtr RegistrySync(const std::string & name = "")
			{
				return GetSync(name);
			}

			template<typename SinkType, typename... Args>
			static LoggerPtr RegistrySync(const std::string & name, Args &&... args)
			{
				return AddSink(RegistrySync(name), std::make_shared<SinkType>(std::forward<Args>(args)...));
			}

			static LoggerPtr RegistrySync(const std::string & name, const SinkPtr & sink)
			{
				return AddSink(RegistrySync(name), sink);
			}

			static LoggerPtr RegistrySync(const std::string & name, const SinkInitList & sinkList)
			{
				return AddSink(RegistrySync(name), sinkList);
			}

			template<class It>
			static LoggerPtr RegistrySync(const std::string & name, const It & begin, const It & end)
			{
				return AddSink(RegistrySync(name), begin, end);
			}

			static LoggerPtr RegistryAsync(const std::string & name = "")
			{
				return GetAsync(name);
			}

			template<typename SinkType, typename... Args>
			static LoggerPtr RegistryAsync(const std::string & name, Args &&... args)
			{
				return AddSink(RegistryAsync(name), std::make_shared<SinkType>(std::forward<Args>(args)...));
			}

			static LoggerPtr RegistryAsync(const std::string & name, const SinkPtr & sink)
			{
				return AddSink(RegistryAsync(name), sink);
			}

			static LoggerPtr RegistryAsync(const std::string & name, const SinkInitList & sinkList)
			{
				return AddSink(RegistryAsync(name), sinkList);
			}

			template<class It>
			static LoggerPtr RegistryAsync(const std::string & name, const It & begin, const It & end)
			{
				return AddSink(RegistryAsync(name), begin, end);
			}

			static LoggerPtr SyslogLogger(const std::string & name, const std::string & idents = "",
																	const int32_t option = 0,
																	const int32_t facility = LOG_USER)
			{
				return RegistrySync<SyslogSink>(name, idents, facility);
			}

			static LoggerPtr NullLoggerSync(const std::string & name)
			{
				return RegistrySync<NullSinkSync>(name);
			}

			static LoggerPtr NullLoggerAsync(const std::string & name)
			{
				return RegistrySync<NullSinkAsync>(name);
			}

			template <typename PathType>
			static LoggerPtr FileLoggerSync(const std::string & name, const PathType & path = "", const bool truncate = false)
			{
				return RegistrySync<FileSinkSync>(name, path, truncate);
			}

			template <typename PathType>
			static LoggerPtr FileLoggerAsync(const std::string & name, const PathType & path = "", const bool truncate = false)
			{
				return RegistryAsync<FileSinkAsync>(name, path, truncate);
			}

			static LoggerPtr OStreamLoggerSync(const std::string & name, std::shared_ptr<std::ostream> os)
			{
				return RegistrySync<OStreamSinkSync>(name, std::move(os));
			}

			static LoggerPtr OStreamLoggerAsync(const std::string & name, std::shared_ptr<std::ostream> os)
			{
				return RegistryAsync<OStreamSinkAsync>(name, std::move(os));
			}

			static LoggerPtr ConsoleLoggerSync(const std::string & name, FILE * file = stdout, const bool autoColor = false)
			{
				return RegistrySync<ConsoleSinkSync>(name, file, autoColor);
			}

			static LoggerPtr ConsoleLoggerAsync(const std::string & name, FILE * file = stdout, const bool autoColor = false)
			{
				return RegistryAsync<ConsoleSinkAsync>(name, file, autoColor);
			}

			template <typename PathType>
			static LoggerPtr DailyFileLoggerSync(const std::string & name,  const PathType & path = "",
																			const int32_t hour = 0,
																			const int32_t minutes = 0,
																			const int32_t seconds = 0)
			{
				return RegistrySync<DailyFileSinkSync>(name, path, hour, minutes, seconds);
			}

			template <typename PathType>
			static LoggerPtr DailyFileLoggerAsync(const std::string & name, const PathType & path = "",
																			const int32_t hour = 0,
																			const int32_t minutes = 0,
																			const int32_t seconds = 0)
			{
				return RegistryAsync<DailyFileSinkAsync>(name, path, hour, minutes, seconds);
			}

			template <typename PathType>
			static LoggerPtr RotatingFileLoggerSync(const std::string & name, const PathType & path = "",
																			  const std::size_t size = TINY_GB,
																			  const std::size_t files = 100)
			{
				return RegistrySync<RotatingFileSinkSync>(name, path, size, files);
			}

			template <typename PathType>
			static LoggerPtr RotatingFileLoggerAsync(const std::string & name,  const PathType & path = "",
																				const std::size_t size = TINY_GB,
																				const std::size_t files = 100)
			{
				return RegistryAsync<RotatingFileSinkAsync>(name, path, size, files);
			}

		protected:
			static LoggerPtr AddSink(const LoggerPtr & logger, const SinkPtr & sink)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				logger->AddSink(sink);

				return logger;
			}

			static LoggerPtr AddSink(const LoggerPtr & logger, const SinkInitList & sinkList)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				logger->AddSink(sinkList);

				return logger;
			}

			template<class It>
			static LoggerPtr AddSink(const LoggerPtr & logger, const It & begin, const It & end)
			{
				TINY_ASSERT(logger, "logger is nullptr")

				logger->AddSink(begin, end);

				return logger;
			}

			static std::string GetLoggerName(const std::string & name)
			{
				std::string prefix(TINY_FILE_APPLICATION_NAME().string());

				if (name.empty())
				{
					return prefix;
				}
				else
				{
					return JoinLoggerName(prefix, name);
				}
			}

			static std::string JoinLoggerName(const std::string & name, const std::string & suffix)
			{
				TINY_ASSERT(!name.empty(), "name empty");
				TINY_ASSERT(!suffix.empty(), "suffix empty");

				return name + '.' + suffix;
			}

			static LoggerPtr GetLoggerByName(TINY_LOG_TYPE type, std::string && name)
			{
				std::lock_guard<std::mutex> lock(_lock);

				auto iter = _loggers.find(name);

				if (iter == _loggers.end())
				{
					if (type == TINY_LOG_TYPE_SYNC)
					{
						_loggers.insert(LoggerMap::value_type(name, std::make_shared<SyncLogger>()));

						iter = _loggers.find(name);
					}
					else
					{
						_loggers.insert(LoggerMap::value_type(name, std::make_shared<AsyncLogger>()));

						iter = _loggers.find(name);
					}
				}

				return iter->second;
			}

		protected:
			static std::mutex _lock;

			static std::map<std::string, LoggerPtr> _loggers;
		};
	}
}





#endif // __TINY_CORE__LOG__REGISTRY__H__
