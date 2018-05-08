#ifndef __TINY_CORE__LOG__FORMATTER__H__
#define __TINY_CORE__LOG__FORMATTER__H__


/**
 *
 *  作者: hm
 *
 *  说明: 日志格式化
 *
 */


#include <tinyCore/log/detail.h>


namespace tinyCore
{
	namespace log
	{
		static const std::string TinyLogDayName[]
		{
			"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
		};

		static const std::string TinyLogFullDayName[]
		{
			"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
		};

		static const std::string TinyLogMonthName[]
		{
			"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
		};

		static const std::string TinyLogFullMonthName[]
		{
			"January", "February", "March", "April", "May", "June",
			"July", "August", "September", "October", "November", "December"
		};

		static const std::string TinyLogLevelName[]
		{
			"TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "CRIT ", "FATAL"
		};

		class ILogFormatter
		{
		public:
			virtual ~ILogFormatter() = default;

			virtual void Format(LogMessage & msg) = 0;
		};

		class NameLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << msg.name;
			}
		};

		class LevelLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << TinyLogLevelName[static_cast<uint8_t>(msg.level)];
			}
		};

		class DayNameLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << TinyLogDayName[msg.tm.tm_wday];
			}
		};

		class FullDayNameLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << TinyLogFullDayName[msg.tm.tm_wday];
			}
		};

		class MonthNameLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << TinyLogMonthName[msg.tm.tm_mon];
			}
		};

		class FullMonthNameLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << TinyLogFullMonthName[msg.tm.tm_mon];
			}
		};

		class TimeFlagLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << (msg.tm.tm_hour >= 12 ? "PM" : "AM");
			}
		};

		// 05:01:04
		class ShortTimeLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::pad(msg.tm.tm_hour, 2, '0')  << ':'
							  << fmt::pad(msg.tm.tm_min,  2, '0') << ':'
							  << fmt::pad(msg.tm.tm_sec,  2, '0');
			}
		};

		// 05:01:04 PM
		class ShortHourTimeFlagLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::pad(msg.tm.tm_hour > 12 ? msg.tm.tm_hour - 12 : msg.tm.tm_hour, 2, '0')  << ':'
							  << fmt::pad(msg.tm.tm_min,  2, '0') << ':'
							  << fmt::pad(msg.tm.tm_sec,  2, '0') << ' '
							  << (msg.tm.tm_hour >= 12 ? "PM" : "AM");
			}
		};

		/// Fri 30 Mar 2018 05:01:04 PM CST
		class DateLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << TinyLogDayName[msg.tm.tm_wday] << ' '
							  << msg.tm.tm_mday << ' '
							  << TinyLogMonthName[msg.tm.tm_mon] << ' '
							  << (msg.tm.tm_year + 1900) << ' '
							  << fmt::pad(msg.tm.tm_hour, 2, '0') << ':'
							  << fmt::pad(msg.tm.tm_min,  2, '0') << ':'
							  << fmt::pad(msg.tm.tm_sec,  2, '0') << ' '
							  << (msg.tm.tm_hour >= 12 ? "PM CST" : "AM CST");
			}
		};

		/// 03/30/18
		class ShortDateLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::pad(msg.tm.tm_mon + 1, 2, '0') << '/'
							  << fmt::pad(msg.tm.tm_mday,  2, '0') << '/'
							  << msg.tm.tm_year % 100;
			}
		};

		/// 2018
		class YearLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << (msg.tm.tm_year + 1900);
			}
		};

		/// 20
		class SubYearLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::pad(msg.tm.tm_year % 100, 2, '0');
			}
		};

		/// 1-12
		class MonthLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::pad(msg.tm.tm_mon + 1, 2, '0');
			}
		};

		/// 1-31
		class DayLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::pad(msg.tm.tm_mday, 2, '0');
			}
		};

		/// 1-31
		class NotFillDayLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << msg.tm.tm_mday;
			}
		};

		/// 1-366
		class YearDayLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << (msg.tm.tm_yday + 1);
			}
		};

		/// 1-366
		class WeakDayLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << msg.tm.tm_wday;
			}
		};

		/// 0-23
		class HourLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::pad(msg.tm.tm_hour, 2, '0');
			}
		};

		/// 0-23
		class NotFillHourLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << msg.tm.tm_hour;
			}
		};

		/// 1-12
		class ShortHourLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::pad(msg.tm.tm_hour > 12 ? msg.tm.tm_hour - 12 : msg.tm.tm_hour, 2, '0');
			}
		};

		/// 1-12
		class ShortNotFillHourLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << (msg.tm.tm_hour > 12 ? msg.tm.tm_hour - 12 : msg.tm.tm_hour);
			}
		};

		/// 0-59
		class MinutesLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::pad(msg.tm.tm_min, 2, '0');
			}
		};

		/// 0-59
		class SecondsLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::pad(msg.tm.tm_sec, 2, '0');
			}
		};

		/// 0-59
		class AllSecondsLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				auto duration = msg.time.time_since_epoch();

				auto second = std::chrono::duration_cast<std::chrono::seconds>(duration).count() % 1000;

				msg.formatted << fmt::pad(second, 2, '0');
			}
		};

		/// 0-999
		class MillisecondsLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				auto duration = msg.time.time_since_epoch();

				auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

				msg.formatted << fmt::pad(milli, 3, '0');
			}
		};

		/// 0-999999
		class MicrosecondsLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				auto duration = msg.time.time_since_epoch();

				auto micro = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000000;

				msg.formatted << fmt::pad(micro, 6, '0');
			}
		};

		/// 0-999999999
		class NanosecondsLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				auto duration = msg.time.time_since_epoch();

				auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() % 1000000000;

				msg.formatted << fmt::pad(ns, 9, '0');
			}
		};

		class CharLogFormatter : public ILogFormatter
		{
		public:
			explicit CharLogFormatter(const char ch) : _ch(ch)
			{

			}

			void Format(LogMessage & msg) override
			{
				msg.formatted << _ch;
			}

		protected:
			char _ch;
		};

		class IDLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::pad(msg.messageID, 6, '0');
			}
		};

		class PIDLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << TINY_ID_PID();
			}
		};

		class ThreadIDLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << msg.threadID;
			}
		};

		class ValueLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				msg.formatted << fmt::StringRef(msg.msg.data(), msg.msg.size());
			}
		};

		class FullLogFormatter : public ILogFormatter
		{
			void Format(LogMessage & msg) override
			{
				auto duration = msg.time.time_since_epoch();

				auto micro = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000000;

				msg.formatted << '['
							  << (msg.tm.tm_year + 1900) << "-"
							  << fmt::pad(msg.tm.tm_mon + 1, 2, '0') << '-'
							  << fmt::pad(msg.tm.tm_mday, 2, '0') << ' '
							  << fmt::pad(msg.tm.tm_hour, 2, '0') << ':'
							  << fmt::pad(msg.tm.tm_min, 2, '0') << ':'
							  << fmt::pad(msg.tm.tm_sec, 2, '0') << '.'
							  << fmt::pad(micro, 6, '0')
							  << ']';

				msg.formatted << '['
							  << msg.name
							  << ']';

				msg.formatted << '['
							  << TinyLogLevelName[static_cast<uint8_t>(msg.level)]
							  << "] ";

				msg.formatted << fmt::StringRef(msg.msg.data(), msg.msg.size());
			}
		};

		class TagFormatter : public ILogFormatter
		{
		public:
			void Add(const char ch)
			{
				_str += ch;
			}

			void Format(LogMessage & msg) override
			{
				msg.formatted << _str;
			}

		protected:
			std::string _str{ };
		};

		class LogFormatter : public ILogFormatter
		{
		public:
			explicit LogFormatter(const std::string & pattern, std::string end = TINY_LOG_END) : _end(std::move(end))
			{
				Compile(pattern);
			}

			void Format(LogMessage & msg) override
			{
				msg.tm = TINY_TIME_TO_LOCAL_TIME(msg.time);

				for (auto &f : _formatterContainer)
				{
					if (f)
					{
						f->Format(msg);
					}
				}

				msg.formatted.write(_end.data(), _end.size());
			}

		protected:
			void Compile(const std::string & pattern)
			{
				auto end = pattern.end();

				std::unique_ptr<TagFormatter> tag;

				for (auto it = pattern.begin(); it != end; ++it)
				{
					if (*it == '%')
					{
						if (tag) // append user chars found so far
						{
							_formatterContainer.push_back(std::move(tag));
						}

						if (++it != end)
						{
							Handle(*it);
						}
						else
						{
							break;
						}
					}
					else // chars not following the % sign should be displayed as is
					{
						if (!tag)
						{
							tag = std::make_unique<TagFormatter>();
						}

						tag->Add(*it);
					}
				}

				if (tag) // append raw chars found so far
				{
					_formatterContainer.push_back(std::move(tag));
				}
			}

			void Handle(const char flag)
			{
				switch (flag)
				{
					case 'a':  /// 当前区域的星期几的简写 (Sun..Sat)
					{
						_formatterContainer.emplace_back(new DayNameLogFormatter());

						break;
					}

					case 'A':  /// 当前区域的星期几的全称 (不同长度) (Sunday..Saturday)
					{
						_formatterContainer.emplace_back(new FullDayNameLogFormatter());

						break;
					}

					case 'b':  /// 当前区域的月份的简写 (Jan..Dec)
					case 'h':  /// 同 %b
					{
						_formatterContainer.emplace_back(new MonthNameLogFormatter());

						break;
					}

					case 'B':  /// 当前区域的月份的全称(变长) (January..December)
					{
						_formatterContainer.emplace_back(new FullMonthNameLogFormatter());

						break;
					}

					case 'c':  /// 当前区域的日期和时间 (Sat Nov 04 12:02:33 EST 1989)
					{
						_formatterContainer.emplace_back(new DateLogFormatter());

						break;
					}

					case 'd':  /// (月份中的)几号(用两位表示) (01..31)
					{
						_formatterContainer.emplace_back(new DayLogFormatter());

						break;
					}

					case 'D':  /// 日期(按照 月/日期/年 格式显示) (mm/dd/yy)
					case 'x':  /// 按照 (mm/dd/yy) 格式显示当前日期
					{
						_formatterContainer.emplace_back(new ShortDateLogFormatter());

						break;
					}

					case 'e':  /// (月份中的)几号(去零表示) (1..31)
					{
						_formatterContainer.emplace_back(new NotFillDayLogFormatter());

						break;
					}

					case 'f':
					{
						_formatterContainer.emplace_back(new MillisecondsLogFormatter());

						break;
					}

					case 'F':
					{
						_formatterContainer.emplace_back(new MicrosecondsLogFormatter());

						break;
					}

					case 'H':  /// 小时(按 24 小时制显示，用两位表示) (00..23)
					{
						_formatterContainer.emplace_back(new HourLogFormatter());

						break;
					}

					case 'i': /// ID
					{
						_formatterContainer.emplace_back(new IDLogFormatter());

						break;
					}

					case 'I':  /// 小时(按 12 小时制显示，用两位表示) (01..12)
					{
						_formatterContainer.emplace_back(new ShortHourLogFormatter());

						break;
					}

					case 'j':  /// (一年中的)第几天(用三位表示) (001..366)
					{
						_formatterContainer.emplace_back(new YearDayLogFormatter());

						break;
					}

					case 'k':  /// 小时(按 24 小时制显示，去零显示) (0..23)
					{
						_formatterContainer.emplace_back(new NotFillHourLogFormatter());

						break;
					}

					case 'l':  /// 小时(按 12 小时制显示，去零表示) (1..12)
					{
						_formatterContainer.emplace_back(new ShortNotFillHourLogFormatter());

						break;
					}

					case 'L': /// Level
					{
						_formatterContainer.emplace_back(new LevelLogFormatter());

						break;
					}

					case 'm':  /// 月份(用两位表示) (01..12)
					{
						_formatterContainer.emplace_back(new MonthLogFormatter());

						break;
					}

					case 'M':  /// 分钟数(用两位表示) (00..59)
					{
						_formatterContainer.emplace_back(new MinutesLogFormatter());

						break;
					}

					case 'n':
					{
						_formatterContainer.emplace_back(new NanosecondsLogFormatter());

						break;
					}

					case 'N': /// Name
					{
						_formatterContainer.emplace_back(new NameLogFormatter());

						break;
					}

					case 'p':  /// 当前时间是上午 AM 还是下午 PM
					{
						_formatterContainer.emplace_back(new TimeFlagLogFormatter());

						break;
					}

					case 'P':  /// PID
					{
						_formatterContainer.emplace_back(new PIDLogFormatter());

						break;
					}

					case 'r':  /// 时间, 按 12 小时制显示 (hh:mm:ss [A/P]M)
					{
						_formatterContainer.emplace_back(new ShortHourTimeFlagLogFormatter());

						break;
					}

					case 's':  /// 从 1970年1月1日0点0分0秒到现在历经的秒数 (GNU扩充)
					{
						_formatterContainer.emplace_back(new AllSecondsLogFormatter());

						break;
					}

					case 'S':  /// 秒数(用两位表示)(00..60)
					{
						_formatterContainer.emplace_back(new SecondsLogFormatter());

						break;
					}

					case 't':  /// thread id
					{
						_formatterContainer.emplace_back(new ThreadIDLogFormatter());

						break;
					}

					case 'T':  /// 时间,按 24 小时制显示(hh:mm:ss)
					case 'X':  /// 按照 (%H:%M:%S) 格式显示当前时间
					{
						_formatterContainer.emplace_back(new ShortTimeLogFormatter());

						break;
					}

					case 'U':  /// (一年中的)第几个星期，以星期天作为一周的开始(用两位表示) (00..53)
					{
						break;
					}

					case 'v': /// Value
					{
						_formatterContainer.emplace_back(new ValueLogFormatter());

						break;
					}

					case 'V':  /// (一年中的)第几个星期，以星期一作为一周的开始(用两位表示) (01..52)
					{
						break;
					}

					case 'w':  /// 用数字表示星期几 (0..6); 0 代表星期天
					{
						_formatterContainer.emplace_back(new WeakDayLogFormatter());

						break;
					}

					case 'W':  /// (一年中的)第几个星期，以星期一作为一周的开始(用两位表示) (00..53)
					{
						break;
					}

					case 'y':  /// 年的后两位数字 (00..99)
					{
						_formatterContainer.emplace_back(new SubYearLogFormatter());

						break;
					}

					case 'Y':  /// 年(用 4 位表示) (1970...)
					{
						_formatterContainer.emplace_back(new YearLogFormatter());

						break;
					}

					case 'z':  /// 按照 RFC-822 中指定的数字时区显示(如, -0500) (为非标准扩充)
					{
						break;
					}

					case 'Z':  /// 时区(例如, EDT (美国东部时区)), 如果不能决定是哪个时区则为空
					{
						break;
					}

					case '+':
					{
						_formatterContainer.emplace_back(new FullLogFormatter());

						break;
					}

					default:
					{
						_formatterContainer.emplace_back(new CharLogFormatter('%'));
						_formatterContainer.emplace_back(new CharLogFormatter(flag));

						break;
					}
				}
			}

		protected:
			const std::string _end{ };

			std::vector<std::unique_ptr<ILogFormatter>> _formatterContainer{ };
		};
	}
}


#endif // __TINY_CORE__LOG__FORMATTER__H__
