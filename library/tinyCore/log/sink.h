#ifndef __TINY_CORE__LOG__SINK__H__
#define __TINY_CORE__LOG__SINK__H__


/**
 *
 *  作者: hm
 *
 *  说明: 日志sink
 *
 */


#include <tinyCore/log/file.h>
#include <tinyCore/lock/mutex.h>


namespace tinyCore
{
	namespace log
	{
		class ISink
		{
		public:
			virtual ~ISink() = default;

			TINY_LOG_LEVEL Level() const
			{
				return _level.load(std::memory_order_relaxed);
			}

			void SetLevel(const TINY_LOG_LEVEL level)
			{
				_level.store(level);
			}

			bool CheckLevel(const TINY_LOG_LEVEL level) const
			{
				return level >= _level.load(std::memory_order_relaxed);
			}

			virtual void Flush() = 0;
			virtual void Write(const LogMessage & msg) = 0;

		protected:
			std::atomic<TINY_LOG_LEVEL> _level { TINY_LOG_LEVEL_TRACE };
		};

		class SyslogSink : public ISink
		{
			using SinkType = SyslogSink;

		public:
			static std::shared_ptr<SinkType> Instance()
			{
				static auto instance = std::make_shared<SinkType>();

				return instance;
			}

			explicit SyslogSink(const std::string & idents = "", const int32_t option = 0, const int32_t facility = LOG_USER) :
					_priorities()
			{
				_priorities[static_cast<uint8_t>(TINY_LOG_LEVEL_TRACE)]    = LOG_DEBUG;
				_priorities[static_cast<uint8_t>(TINY_LOG_LEVEL_DEBUG)]    = LOG_DEBUG;
				_priorities[static_cast<uint8_t>(TINY_LOG_LEVEL_INFO)]     = LOG_INFO;
				_priorities[static_cast<uint8_t>(TINY_LOG_LEVEL_WARNING)]  = LOG_WARNING;
				_priorities[static_cast<uint8_t>(TINY_LOG_LEVEL_ERROR)]    = LOG_ERR;
				_priorities[static_cast<uint8_t>(TINY_LOG_LEVEL_CRITICAL)] = LOG_CRIT;
				_priorities[static_cast<uint8_t>(TINY_LOG_LEVEL_FATAL)]    = LOG_INFO;

				// 如果为空, 将身份识别设置为程序名
				::openlog(idents.empty() ? nullptr : idents.c_str(), option, facility);
			}

			~SyslogSink() override
			{
				Flush();

				::closelog();
			}

			void Flush() override
			{

			}

			void Write(const LogMessage & msg) override
			{
				::syslog(_priorities[static_cast<uint8_t>(msg.level)], "%s", msg.msg.c_str());
			}

		protected:
			std::array<int32_t, 7> _priorities{ };
		};

		template<class MutexType>
		class BaseSink : public ISink
		{
			using SinkType = BaseSink<MutexType>;

		public:
			~BaseSink() override = default;

			void Flush() override
			{
				std::lock_guard<MutexType> lock(_mutex);

				FlushSink();
			}

			void Write(const LogMessage & msg) override
			{
				std::lock_guard<MutexType> lock(_mutex);

				WriteSink(msg);
			}

		protected:
			virtual void FlushSink() = 0;
			virtual void WriteSink(const LogMessage & msg) = 0;

		protected:
			MutexType _mutex{ };
		};

		template<class MutexType>
		class NullSink : public BaseSink<MutexType>
		{
			using SinkType = NullSink<MutexType>;

		public:
			static std::shared_ptr<SinkType> Instance()
			{
				static auto instance = std::make_shared<SinkType>();

				return instance;
			}

			~NullSink()
			{
				FlushSink();
			}

		protected:
			void FlushSink() override
			{

			}

			void WriteSink(const LogMessage & msg) override
			{

			}
		};

		template<class MutexType>
		class FileSink : public BaseSink<MutexType>
		{
			using SinkType = FileSink<MutexType>;

		public:
			static std::shared_ptr<SinkType> Instance()
			{
				static auto instance = std::make_shared<SinkType>
				(
					TINY_STR_FORMAT("{}/log/{}.log", TINY_FILE_APPLICATION_PARENT_PATH().string(),
													 TINY_FILE_APPLICATION_NAME().string())
				);

				return instance;
			}

			template <typename PathType>
			explicit FileSink(const PathType & path, const bool truncate = false)
			{
				_logFile.Open(path, truncate);
			}

			~FileSink()
			{
				FlushSink();
			}

			void SetAutoFlush(const bool autoFlush)
			{
				_autoFlush = autoFlush;
			}

		protected:
			void FlushSink() override
			{
				_logFile.Flush();
			}

			void WriteSink(const LogMessage & msg) override
			{
				_logFile.Write(msg);

				if (_autoFlush)
				{
					_logFile.Flush();
				}
			}

		protected:
			bool _autoFlush{ false };

			LogFile _logFile{ };
		};

		template<class MutexType>
		class DailyFileSink : public BaseSink<MutexType>
		{
			using SinkType = DailyFileSink<MutexType>;

		public:
			static std::shared_ptr<SinkType> Instance()
			{
				static auto instance = std::make_shared<SinkType>
				(
					TINY_STR_FORMAT("{}/log/{}.log", TINY_FILE_APPLICATION_PARENT_PATH().string(),
													 TINY_FILE_APPLICATION_NAME().string())
				);

				return instance;
			}

			template <typename PathType>
			explicit DailyFileSink(const PathType & path, const int32_t hour = 0,
														  const int32_t minutes = 0,
														  const int32_t seconds = 0) : _hour(hour),
																					   _minutes(minutes),
																					   _seconds(seconds),
																					   _path(path)
			{
				if (hour < 0 || hour > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59)
				{
					TINY_THROW_EXCEPTION(debug::ValueError, "Invalid Time")
				}

				_logFile.Open
				(
					TINY_STR_FORMAT("{}/{}_{}{}",
									_path.parent_path().empty() ? TINY_FILE_APPLICATION_PARENT_PATH().string() :
																  _path.parent_path().string(),
									_path.stem().string(),
									TINY_TIME_CURRENT_TIME_STRING("%Y_%m_%d_%H_%M_%S"),
									_path.extension().string())
				);

				RotatingTime();
			}

			~DailyFileSink()
			{
				FlushSink();
			}

			void SetAutoFlush(const bool autoFlush)
			{
				_autoFlush = autoFlush;
			}

		protected:
			void RotatingTime()
			{
				std::tm date = TINY_TIME_LOCAL_TIME();

				date.tm_hour = _hour;
				date.tm_min  = _minutes;
				date.tm_sec  = _seconds;

				_time = TINY_TIME_FROM_TIME(date);

				if (_time < TINY_TIME_SECONDS())
				{
					_time += TINY_DAY;
				}
			}

			void FlushSink() override
			{
				_logFile.Flush();
			}

			void WriteSink(const LogMessage & msg) override
			{
				if (TINY_TIME_SECONDS() >= _time)
				{
					_logFile.Reopen
					(
						TINY_STR_FORMAT("{}/{}_{}{}", _path.parent_path().string(),
													  _path.stem().string(),
													  TINY_TIME_CURRENT_TIME_STRING("%Y_%m_%d_%H_%M_%S"),
													  _path.extension().string())
					);

					RotatingTime();
				}

				_logFile.Write(msg);

				if (_autoFlush)
				{
					_logFile.Flush();
				}
			}

		protected:
			bool _autoFlush{ false };

			std::time_t _time{ 0 };

			std::int32_t _hour{ 0 };
			std::int32_t _minutes{ 0 };
			std::int32_t _seconds{ 0 };

			LogFile _logFile{ };

			system::FileSystem::PathInfo _path{ };
		};

		template<class MutexType>
		class RotatingFileSink : public BaseSink<MutexType>
		{
			using SinkType = RotatingFileSink<MutexType>;

		public:
			static std::shared_ptr<SinkType> Instance()
			{
				static auto instance = std::make_shared<SinkType>
				(
					TINY_STR_FORMAT("{}/log/{}.log", TINY_FILE_APPLICATION_PARENT_PATH().string(),
													 TINY_FILE_APPLICATION_NAME().string())
				);

				return instance;
			}

			template <typename PathType>
			explicit RotatingFileSink(const PathType & path, const size_t size = TINY_GB, const size_t files = 100) :
					_size(size),
					_files(files)
			{
				_logFile.Open(path);
			}

			~RotatingFileSink()
			{
				FlushSink();
			}

			void SetAutoFlush(const bool autoFlush)
			{
				_autoFlush = autoFlush;
			}

		protected:
			void Rotating()
			{
				_logFile.Close();

				for (std::size_t i = _files; i > 0; --i)
				{
					system::FileSystem::PathInfo src = HandleFileName(i - 1);
					system::FileSystem::PathInfo dst = HandleFileName(i);

					if (TINY_FILE_IS_EXISTS(dst))
					{
						if (!TINY_FILE_REMOVE(dst))
						{
							TINY_THROW_EXCEPTION(debug::FileError, TINY_STR_FORMAT("Failed Removing {}", dst.string()))
						}
					}

					if (TINY_FILE_IS_EXISTS(src))
					{
						TINY_FILE_RENAME(src, dst);
					}
				}

				_logFile.Reopen(_logFile.Path(), true);
			}

			system::FileSystem::PathInfo HandleFileName(const std::size_t index)
			{
				if (index == 0)
				{
					return _logFile.Path();
				}
				else
				{
					return TINY_STR_FORMAT("{}/{}_{}{}",
										   _logFile.Path().parent_path().string(),
										   _logFile.Path().stem().string(),
										   index,
										   _logFile.Path().extension().string());
				}
			}

			void FlushSink() override
			{
				_logFile.Flush();
			}

			void WriteSink(const LogMessage & msg) override
			{
				_logFile.Write(msg);

				if (_autoFlush)
				{
					_logFile.Flush();
				}

				if (_logFile.Size() > _size)
				{
					Rotating();
				}
			}

		protected:
			bool _autoFlush{ false };

			std::size_t _size{ 0 };
			std::size_t _files{ 0 };

			LogFile _logFile{ };
		};

		template<class MutexType>
		class OStreamSink : public BaseSink<MutexType>
		{
			using SinkType = OStreamSink<MutexType>;

		public:
			static std::shared_ptr<SinkType> Instance()
			{
				static auto instance = std::make_shared<SinkType>(std::make_shared<std::ostringstream>());

				return instance;
			}

			explicit OStreamSink(std::shared_ptr<std::ostream> os) : _stream(std::move(os))
			{

			}

			~OStreamSink()
			{
				FlushSink();
			}

			void SetAutoFlush(const bool autoFlush)
			{
				_autoFlush = autoFlush;
			}

			std::shared_ptr<std::ostream> Stream() const
			{
				return _stream;
			}

		protected:
			void FlushSink() override
			{
				if (_stream)
				{
					_stream->flush();
				}
			}

			void WriteSink(const LogMessage & msg) override
			{
				if (_stream)
				{
					_stream->write(msg.formatted.data(), msg.formatted.size());

					if (_autoFlush)
					{
						_stream->flush();
					}
				}
			}

		protected:
			bool _autoFlush{ false };

			std::shared_ptr<std::ostream>_stream{ };
		};

		template<class MutexType>
		class ConsoleSink : public BaseSink<MutexType>
		{
			using SinkType = ConsoleSink<MutexType>;

		public:
			static std::shared_ptr<SinkType> Instance()
			{
				static auto instance = std::make_shared<SinkType>(stdout);

				return instance;
			}

			explicit ConsoleSink(FILE * file, const bool autoColor = false) : _autoColor(autoColor),
																			  _console(file)
			{
				_canColor = IsTerminal(file) && IsColorTerminal();

				_colors[TINY_LOG_LEVEL_TRACE]    = ForegroundCyan;                          // 前景绿色
				_colors[TINY_LOG_LEVEL_DEBUG]    = ForegroundCyan;                          // 前景绿色
				_colors[TINY_LOG_LEVEL_INFO]     = FormattingReset;                         // 关闭所有属性
				_colors[TINY_LOG_LEVEL_WARNING]  = ForegroundYellow + FormattingHighLight;  // 前景黄色高亮度
				_colors[TINY_LOG_LEVEL_ERROR]    = ForegroundRed    + FormattingHighLight;  // 前景红色高亮度
				_colors[TINY_LOG_LEVEL_CRITICAL] = ForegroundRed    + FormattingHighLight;  // 前景红色高亮度
				_colors[TINY_LOG_LEVEL_FATAL]    = BackgroundRed    + FormattingHighLight;  // 背景红色高亮度
			}

			~ConsoleSink()
			{
				FlushSink();
			}

			void SetAutoColor(const bool autoColor)
			{
				_autoColor = autoColor;
			}

			void SetColor(const TINY_LOG_LEVEL level, const std::string & color)
			{
				std::lock_guard<MutexType> lock(BaseSink<MutexType>::_mutex);

				_colors[level] = color;
			}

		protected:
			void FlushSink() override
			{
				fflush(_console);
			}

			void WriteSink(const LogMessage & msg) override
			{
				if (_autoColor && _canColor)
				{
					const std::string & prefix = _colors[msg.level];

					fwrite(prefix.data(),              sizeof(char), prefix.size(),              _console);
					fwrite(msg.formatted.data(),       sizeof(char), msg.formatted.size(),       _console);
					fwrite(FormattingReset.data(),     sizeof(char), FormattingReset.size(),     _console);
					fwrite(FormattingClearLine.data(), sizeof(char), FormattingClearLine.size(), _console);
				}
				else
				{
					fwrite(msg.formatted.data(), sizeof(char), msg.formatted.size(), _console);
				}

				FlushSink();
			}

			bool IsTerminal(FILE * file)
			{
			#if TINY_PLATFORM == TINYPLATFORM_WINDOWS

				return _isatty(_fileno(file)) != 0;

			#else

				return isatty(fileno(file)) != 0;

			#endif
			}

			bool IsColorTerminal()
			{
			#if TINY_PLATFORM == TINY_PLATFORM_WINDOWS

				return true;

			#else

				static constexpr const char * Terms[] =
				{
					"ansi",  "color", "console", "cygwin", "gnome",  "konsole", "kterm",
					"linux", "msys",  "putty",   "rxvt",   "screen", "vt100",   "xterm"
				};

				const char * env_p = std::getenv("TERM");  // 从环境中取字符串,获取环境变量的值

				if (env_p == nullptr)
				{
					return false;
				}

				static const bool result = std::any_of
				(
					std::begin(Terms), std::end(Terms), [&](const char * term)
														{
															return std::strstr(env_p, term) != nullptr;
														}
				);

				return result;

			#endif
			}

		public:
			/// Formatting codes
			const std::string FormattingReset     = "\033[0m";  // 关闭所有属性
			const std::string FormattingHighLight = "\033[1m";  // 高亮度
			const std::string FormattingDark      = "\033[2m";
			const std::string FormattingUnderline = "\033[4m";  // 下划线
			const std::string FormattingBlink     = "\033[5m";  // 闪烁
			const std::string FormattingReverse   = "\033[7m";  // 反显
			const std::string FormattingConcealed = "\033[8m";  // 消隐
			const std::string FormattingClearLine = "\033[K";   // 清除从光标到行尾的内容

			/// Foreground colors
			const std::string ForegroundBlack   = "\033[30m";  // 黑色
			const std::string ForegroundRed     = "\033[31m";  // 红色
			const std::string ForegroundGreen   = "\033[32m";  // 绿色
			const std::string ForegroundYellow  = "\033[33m";  // 黄色
			const std::string ForegroundBlue    = "\033[34m";  // 蓝色
			const std::string ForegroundMagenta = "\033[35m";  // 紫色
			const std::string ForegroundCyan    = "\033[36m";  // 深绿色
			const std::string ForegroundWhite   = "\033[37m";  // 白色

			/// Background colors
			const std::string BackgroundBlack   = "\033[40m";  // 黑色
			const std::string BackgroundRed     = "\033[41m";  // 红色
			const std::string BackgroundGreen   = "\033[42m";  // 绿色
			const std::string BackgroundYellow  = "\033[43m";  // 黄色
			const std::string BackgroundBlue    = "\033[44m";  // 蓝色
			const std::string BackgroundMagenta = "\033[45m";  // 紫色
			const std::string BackgroundCyan    = "\033[46m";  // 深绿色
			const std::string BackgroundWhite   = "\033[47m";  // 白色

		protected:
			bool _canColor{ false };
			bool _autoColor{ false };

			FILE * _console{ nullptr };

			std::unordered_map<TINY_LOG_LEVEL, std::string, std::hash<TINY_LOG_LEVEL>> _colors{ };
		};

		template<class MutexType>
		class ManagerSink : public BaseSink<MutexType>
		{
			using SinkPtr = std::shared_ptr<ISink>;
			using SinkType = ManagerSink<MutexType>;
			using SinkVector = std::vector<SinkPtr>;
			using SinkInitList = std::initializer_list<SinkPtr>;

		public:
			static std::shared_ptr<SinkType> Instance()
			{
				static std::shared_ptr<SinkType> instance = std::make_shared<SinkType>();

				return instance;
			}

			ManagerSink() = default;

			explicit ManagerSink(const SinkPtr & sink) : ManagerSink({ sink })
			{

			}

			ManagerSink(const SinkInitList & sinkList) : ManagerSink(sinkList.begin(), sinkList.end())
			{

			}

			template<class It>
			explicit ManagerSink(const It & begin, const It & end) : _sinkVector(begin, end)
			{

			}

			~ManagerSink()
			{
				FlushSink();
			}

			void Add(const SinkPtr & sink)
			{
				std::lock_guard<MutexType> lock(BaseSink<MutexType>::_mutex);

				_sinkVector.push_back(sink);
			}

			void Add(const SinkInitList & sinkList)
			{
				std::lock_guard<MutexType> lock(BaseSink<MutexType>::_mutex);

				_sinkVector.insert(_sinkVector.end(), sinkList.begin(), sinkList.end());
			}

			template<class It>
			void Add(const It & begin, const It & end)
			{
				std::lock_guard<MutexType> lock(BaseSink<MutexType>::_mutex);

				_sinkVector.insert(_sinkVector.end(), begin, end);
			}

			void Remove()
			{
				std::lock_guard<MutexType> lock(BaseSink<MutexType>::_mutex);

				_sinkVector.clear();
			}

			void Remove(const SinkPtr & sink)
			{
				std::lock_guard<MutexType> lock(BaseSink<MutexType>::_mutex);

				_sinkVector.erase(std::remove(_sinkVector.begin(), _sinkVector.end(), sink), _sinkVector.end());
			}

			std::size_t Size() const
			{
				return _sinkVector.size();
			}

			const SinkVector & Sinks() const
			{
				return _sinkVector;
			}

		protected:
			void FlushSink() override
			{
				for (auto &sink : _sinkVector)
				{
					sink->Flush();
				}
			}

			void WriteSink(const LogMessage & msg) override
			{
				for (auto &sink : _sinkVector)
				{
					if (sink->CheckLevel(msg.level))
					{
						sink->Write(msg);
					}
				}
			}

		protected:
			SinkVector _sinkVector{ };
		};
	}
}


using NullSinkSync = tinyCore::log::NullSink<tinyCore::lock::NullMutex>;
using FileSinkSync = tinyCore::log::FileSink<tinyCore::lock::NullMutex>;
using ManagerSinkSync = tinyCore::log::ManagerSink<tinyCore::lock::NullMutex>;
using OStreamSinkSync = tinyCore::log::OStreamSink<tinyCore::lock::NullMutex>;
using ConsoleSinkSync = tinyCore::log::ConsoleSink<tinyCore::lock::NullMutex>;
using DailyFileSinkSync = tinyCore::log::DailyFileSink<tinyCore::lock::NullMutex>;
using RotatingFileSinkSync = tinyCore::log::RotatingFileSink<tinyCore::lock::NullMutex>;

using NullSinkAsync = tinyCore::log::NullSink<tinyCore::lock::SystemMutex>;
using FileSinkAsync = tinyCore::log::FileSink<tinyCore::lock::SystemMutex>;
using ManagerSinkAsync = tinyCore::log::ManagerSink<tinyCore::lock::SystemMutex>;
using OStreamSinkAsync = tinyCore::log::OStreamSink<tinyCore::lock::SystemMutex>;
using ConsoleSinkAsync = tinyCore::log::ConsoleSink<tinyCore::lock::SystemMutex>;
using DailyFileSinkAsync = tinyCore::log::DailyFileSink<tinyCore::lock::SystemMutex>;
using RotatingFileSinkAsync = tinyCore::log::RotatingFileSink<tinyCore::lock::SystemMutex>;


#define sSyslogSink SyslogSink::Instance()

#define sNullSinkSync NullSinkSync::Instance()
#define sFileSinkSync FileSinkSync::Instance()
#define sManagerSinkSync ManagerSinkSync::Instance()
#define sConsoleSinkSync ConsoleSinkSync::Instance()
#define sOStreamSinkSync OStreamSinkSync::Instance()
#define sDailyFileSinkSync DailyFileSinkSync::Instance()
#define sRotatingFileSinkSync RotatingFileSinkSync::Instance()

#define sNullSinkAsync NullSinkAsync::Instance()
#define sFileSinkAsync FileSinkAsync::Instance()
#define sManagerSinkAsync ManagerSinkAsync::Instance()
#define sConsoleSinkAsync ConsoleSinkAsync::Instance()
#define sOStreamSinkAsync OStreamSinkAsync::Instance()
#define sDailyFileSinkAsync DailyFileSinkAsync::Instance()
#define sRotatingFileSinkAsync RotatingFileSinkAsync::Instance()


#endif // __TINY_CORE__LOG__SINK__H__
