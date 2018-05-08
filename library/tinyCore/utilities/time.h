#ifndef __TINY_CORE__UTILITIES__TIME__H__
#define __TINY_CORE__UTILITIES__TIME__H__


/**
 *
 *  作者: hm
 *
 *  说明: 时间处理
 *
 */


#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace utilities
	{
		class TINY_API Time
		{
		public:
			static std::tm UTCTime()
			{
				return ToUTCTime(Seconds());
			}

			static std::tm LocalTime()
			{
				return ToLocalTime(Seconds());
			}

			static std::tm ToUTCTime(const std::time_t time)
			{
				std::tm tm = { };

			#if TINY_PLATFORM == TINY_PLATFORM_WINDOWS

				gmtime_s(&tm, &time);

			#else

				gmtime_r(&time, &tm);

			#endif

				return tm;
			}

			static std::tm ToUTCTime(const SystemClockTimesPoint & time)
			{
				return ToUTCTime(SystemClock::to_time_t(time));
			}

			static std::tm ToLocalTime(const std::time_t time)
			{
				std::tm tm = { };

			#if TINY_PLATFORM == TINY_PLATFORM_WINDOWS

				localtime_s(&tm, &time);

			#else

				localtime_r(&time, &tm);

			#endif

				return tm;
			}

			static std::tm ToLocalTime(const SystemClockTimesPoint & time)
			{
				return ToLocalTime(SystemClock::to_time_t(time));
			}

			static double Double()
			{
				return Double(TimePoint());
			}

			static double Double(const std::time_t time)
			{
				return Double(ToTimePoint(time));
			}

			static double Double(const SystemClockTimesPoint & timesPoint)
			{
				return Double(timesPoint.time_since_epoch());
			}

			static double Double(const SystemClockDuration & duration)
			{
				using type = std::chrono::duration<double>;

				return std::chrono::duration_cast<type>(duration).count();
			}

			static std::time_t Days()
			{
				return Days(TimePoint());
			}

			static std::time_t Days(const std::time_t time)
			{
				return Days(ToTimePoint(time));
			}

			static std::time_t Days(const SystemClockTimesPoint & timesPoint)
			{
				return Days(timesPoint.time_since_epoch());
			}

			static std::time_t Days(const SystemClockDuration & duration)
			{
				using type = std::chrono::duration<int64_t, std::ratio<60 * 60 * 24>>;

				return std::chrono::duration_cast<type>(duration).count();
			}

			static std::time_t Hours()
			{
				return Hours(TimePoint());
			}

			static std::time_t Hours(const std::time_t time)
			{
				return Hours(ToTimePoint(time));
			}

			static std::time_t Hours(const SystemClockTimesPoint & timesPoint)
			{
				return Hours(timesPoint.time_since_epoch());
			}

			static std::time_t Hours(const SystemClockDuration & duration)
			{
				return std::chrono::duration_cast<std::chrono::hours>(duration).count();
			}

			static std::time_t Minutes()
			{
				return Minutes(TimePoint());
			}

			static std::time_t Minutes(const std::time_t time)
			{
				return Minutes(ToTimePoint(time));
			}

			static std::time_t Minutes(const SystemClockTimesPoint & timesPoint)
			{
				return Minutes(timesPoint.time_since_epoch());
			}

			static std::time_t Minutes(const SystemClockDuration & duration)
			{
				return std::chrono::duration_cast<std::chrono::minutes>(duration).count();
			}

			static std::time_t Seconds()
			{
				return Seconds(TimePoint());
			}

			static std::time_t Seconds(const std::time_t time)
			{
				return Seconds(ToTimePoint(time));
			}

			static std::time_t Seconds(const SystemClockTimesPoint & timesPoint)
			{
				return Seconds(timesPoint.time_since_epoch());
			}

			static std::time_t Seconds(const SystemClockDuration & duration)
			{
				return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
			}

			static std::time_t TimeZone()
			{
				return (boost::posix_time::to_time_t(boost::posix_time::microsec_clock::local_time()) -
						boost::posix_time::to_time_t(boost::posix_time::microsec_clock::universal_time())) / 3600;
			}

			static std::time_t Milliseconds()
			{
				return Milliseconds(TimePoint());
			}

			static std::time_t Milliseconds(const std::time_t time)
			{
				return Milliseconds(ToTimePoint(time));
			}

			static std::time_t Milliseconds(const SystemClockTimesPoint & timesPoint)
			{
				return Milliseconds(timesPoint.time_since_epoch());
			}

			static std::time_t Milliseconds(const SystemClockDuration & duration)
			{
				return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			}

			static std::time_t Microseconds()
			{
				return Microseconds(TimePoint());
			}

			static std::time_t Microseconds(const std::time_t time)
			{
				return Microseconds(ToTimePoint(time));
			}

			static std::time_t Microseconds(const SystemClockTimesPoint & timesPoint)
			{
				return Microseconds(timesPoint.time_since_epoch());
			}

			static std::time_t Microseconds(const SystemClockDuration & duration)
			{
				return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
			}

			static std::time_t Nanoseconds()
			{
				return Nanoseconds(TimePoint());
			}

			static std::time_t Nanoseconds(const std::time_t time)
			{
				return Nanoseconds(ToTimePoint(time));
			}

			static std::time_t Nanoseconds(const SystemClockTimesPoint & timesPoint)
			{
				return Nanoseconds(timesPoint.time_since_epoch());
			}

			static std::time_t Nanoseconds(const SystemClockDuration & duration)
			{
				return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
			}

			static std::time_t NextDayTime()
			{
				return CurrentDayTime() + TINY_DAY;
			}

			static std::time_t CurrentDayTime()
			{
				return (((Days() * 24) - TimeZone()) * TINY_HOUR);
			}

			static std::time_t FromTime(std::tm & date)
			{
				return std::mktime(&date);
			}

			template <typename TypeT>
			static std::time_t FromTimeString(const TypeT & value, const char * format = "%Y-%m-%d %H:%M:%S")
			{
				return FromZoneTimeString(value, TimeZone(), format);
			}

			template <typename TypeT>
			static std::time_t FromUTCTimeString(const TypeT & value, const char * format = "%Y-%m-%d %H:%M:%S")
			{
				return FromZoneTimeString(value, 0, format);
			}

			template <typename TypeT>
			static std::time_t FromZoneTimeString(const TypeT & value, const std::time_t timeZone, const char * format = "%Y-%m-%d %H:%M:%S")
			{
				return boost::posix_time::to_time_t(ToPtime(value, format) - boost::posix_time::hours(timeZone));
			}

			static std::string CurrentTimeString(const char * format = "%Y-%m-%d %H:%M:%S")
			{
				std::ostringstream ss;

				ss.imbue(std::locale(std::cin.getloc(), new boost::posix_time::time_facet(format)));

				ss << boost::posix_time::microsec_clock::local_time();

				return ss.str();
			}

			static std::string CurrentUTCTimeString(const char * format = "%Y-%m-%d %H:%M:%S")
			{
				std::ostringstream ss;

				ss.imbue(std::locale(std::cin.getloc(), new boost::posix_time::time_facet(format)));

				ss << boost::posix_time::microsec_clock::universal_time();

				return ss.str();
			}

			static std::string CurrentZoneTimeString(const char * timeZone, const char * format = "%Y-%m-%d %H:%M:%S")
			{
				return FormatZoneTimeString(Seconds(), timeZone, format);
			}

			static std::string FormatTimeString(const std::time_t seconds, const char * format = "%Y-%m-%d %H:%M:%S")
			{
				return FormatZoneTimeString(seconds, "UTC+8", format);
			}

			static std::string FormatUTCTimeString(const std::time_t seconds, const char * format = "%Y-%m-%d %H:%M:%S")
			{
				return FormatZoneTimeString(seconds, "UTC+0", format);
			}

			static std::string FormatZoneTimeString(const std::time_t seconds, const char * timeZone, const char * format = "%Y-%m-%d %H:%M:%S")
			{
				std::ostringstream ss;

				ss.imbue(std::locale(std::cin.getloc(), new boost::posix_time::time_facet(format)));

				boost::local_time::time_zone_ptr timeZonePtr(new boost::local_time::posix_time_zone(timeZone));

				ss << boost::local_time::local_date_time(boost::posix_time::from_time_t(seconds), timeZonePtr).local_time();

				return ss.str();
			}

			template <typename TypeT>
			static BoostPtime ToPtime(const TypeT & value, const char * format = "%Y-%m-%d %H:%M:%S")
			{
				std::istringstream ss(value);

				ss.imbue(std::locale(std::cin.getloc(), new boost::posix_time::time_input_facet(format)));

				BoostPtime temp;

				ss >> temp;

				return temp;
			}

			static SystemClockTimesPoint TimePoint()
			{
			#if TINY_PLATFORM == TINY_PLATFORM_UNIX

				timespec ts = { };

				::clock_gettime(CLOCK_REALTIME_COARSE, &ts);

				return SystemClockTimesPoint
						(
							std::chrono::duration_cast<SystemClockDuration>
							(
								std::chrono::seconds(ts.tv_sec) + std::chrono::nanoseconds(ts.tv_nsec)
							)
						);

			#else

				return std::chrono::system_clock::now();

			#endif
			}

			static SystemClockTimesPoint ToTimePoint(const std::time_t time)
			{
				return std::chrono::system_clock::from_time_t(time);
			}
		};
	}
}


#define TINY_TIME_DAYS(...) tinyCore::utilities::Time::Days(__VA_ARGS__)
#define TINY_TIME_HOURS(...) tinyCore::utilities::Time::Hours(__VA_ARGS__)
#define TINY_TIME_DOUBLE(...) tinyCore::utilities::Time::Double(__VA_ARGS__)
#define TINY_TIME_MINUTES(...) tinyCore::utilities::Time::Minutes(__VA_ARGS__)
#define TINY_TIME_SECONDS(...) tinyCore::utilities::Time::Seconds(__VA_ARGS__)
#define TINY_TIME_NANOSECONDS(...) tinyCore::utilities::Time::Nanoseconds(__VA_ARGS__)
#define TINY_TIME_MILLISECONDS(...) tinyCore::utilities::Time::Milliseconds(__VA_ARGS__)
#define TINY_TIME_MICROSECONDS(...) tinyCore::utilities::Time::Microseconds(__VA_ARGS__)

#define TINY_TIME_ZONE() tinyCore::utilities::Time::TimeZone()
#define TINY_TIME_POINT() tinyCore::utilities::Time::TimePoint()
#define TINY_TIME_NEXT_DAY_TIME() tinyCore::utilities::Time::NextDayTime()
#define TINY_TIME_CURRENT_DAY_TIME() tinyCore::utilities::Time::CurrentDayTime()

#define TINY_TIME_UTC_TIME() tinyCore::utilities::Time::UTCTime()
#define TINY_TIME_LOCAL_TIME() tinyCore::utilities::Time::LocalTime()

#define TINY_TIME_TO_UTC_TIME(value) tinyCore::utilities::Time::ToUTCTime(value)
#define TINY_TIME_TO_LOCAL_TIME(value) tinyCore::utilities::Time::ToLocalTime(value)

#define TINY_TIME_CURRENT_TIME_STRING(...) tinyCore::utilities::Time::CurrentTimeString(__VA_ARGS__)
#define TINY_TIME_CURRENT_UTC_TIME_STRING(...) tinyCore::utilities::Time::CurrentUTCTimeString(__VA_ARGS__)
#define TINY_TIME_CURRENT_ZONE_TIME_STRING(value, ...) tinyCore::utilities::Time::CurrentZoneTimeString(value, ##__VA_ARGS__)

#define TINY_TIME_FROM_TIME(value) tinyCore::utilities::Time::FromTime(value)
#define TINY_TIME_FROM_TIME_STRING(value, ...) tinyCore::utilities::Time::FromTimeString(value, ##__VA_ARGS__)
#define TINY_TIME_FROM_UTC_TIME_STRING(value, ...) tinyCore::utilities::Time::FromUTCTimeString(value, ##__VA_ARGS__)
#define TINY_TIME_FROM_ZONE_TIME_STRING(value, zone, ...) tinyCore::utilities::Time::FromZoneTimeString(value, zone, ##__VA_ARGS__)

#define TINY_TIME_FORMAT_TIME_STRING(seconds, ...) tinyCore::utilities::Time::FormatTimeString(seconds, ##__VA_ARGS__)
#define TINY_TIME_FORMAT_UTC_TIME_STRING(seconds, ...) tinyCore::utilities::Time::FormatUTCTimeString(seconds, ##__VA_ARGS__)
#define TINY_TIME_FORMAT_ZONE_TIME_STRING(seconds, timeZone, ...) tinyCore::utilities::Time::FormatZoneTimeString(seconds, timeZone, ##__VA_ARGS__)

#define TINY_TIME_TO_TIME_POINT(value) tinyCore::utilities::Time::ToTimePoint(value)
#define TINY_TIME_TO_BOOST_PTIME(value, ...) tinyCore::utilities::Time::ToPtime(value, ##__VA_ARGS__)


#endif // __TINY_CORE__UTILITIES__TIME__H__
