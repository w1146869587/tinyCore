#ifndef __TINY_CORE__COMMON__COMMON__H__
#define __TINY_CORE__COMMON__COMMON__H__


// c
#include <ctime>
#include <cmath>
#include <cstdio>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <cstdint>
#include <fcntl.h>
#include <sys/stat.h>

// std
#include <set>
#include <map>
#include <any>
#include <list>
#include <mutex>
#include <array>
#include <queue>
#include <stack>
#include <regex>
#include <mutex>
#include <atomic>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <future>
#include <utility>
#include <numeric>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <condition_variable>
#include <experimental/filesystem>

// zlib
#include <zlib.h>

// boost
#include <boost/format.hpp>
#include <boost/config.hpp>
#include <boost/phoenix.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/date_time.hpp>
#include <boost/mpl/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/checked_delete.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/functional/factory.hpp>

// rapidjson
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include <rapidjson/pointer.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

// common
#include <tinyCore/common/macro.h>
#include <tinyCore/common/version.h>
#include <tinyCore/common/platform.h>
#include <tinyCore/common/compiler.h>

//fmt
#include <fmt/fmt/time.h>
#include <fmt/fmt/posix.h>
#include <fmt/fmt/format.h>
#include <fmt/fmt/ostream.h>


#if TINY_PLATFORM == TINY_PLATFORM_WINDOWS
#
#  include <ws2tcpip.h>
#
#  if TINY_COMPILER == TINY_COMPILER_INTEL
#
#    if !defined(BOOST_ASIO_HAS_MOVE)
#
#      define BOOST_ASIO_HAS_MOVE
#
#    endif // BOOST_ASIO_HAS_MOVE
#
#  endif // TINY_COMPILER
#
#else
#
#  include <netdb.h>
#  include <unistd.h>
#  include <syslog.h>
#  include <syscall.h>
#  include <execinfo.h>
#
#  include <net/if.h>
#
#  include <sys/mman.h>
#  include <sys/types.h>
#  include <sys/ioctl.h>
#  include <sys/socket.h>
#
#  include <scsi/sg.h>
#
#  include <arpa/inet.h>
#
#  include <linux/hdreg.h>
#
#  include <netinet/in.h>
#
#endif


/**
 *
 * debug选项
 *
 */
#ifndef NDEBUG
#
#	ifndef _DEBUG
#
#		define _DEBUG
#
#	endif
#
#	ifndef DEBUG
#
#		define DEBUG
#
#	endif
#
#   ifndef _GLIBCXX_DEBUG
#
#       define _GLIBCXX_DEBUG
#
#   endif
#
#endif


/**
 *
 * DECLSPEC_NORETURN 告诉编译器, 这个函数不会返回, 让编译器知道调用约定为__declspec(noreturn)的函数之后的代码不可到达
 *
 * DECLSPEC_DEPRECATED 说明一个函数, 类型, 或别的标识符在新的版本或未来版本中不再支持, 你不应该用这个函数或类型
 *
 */
#if TINY_PLATFORM == TINY_PLATFORM_WINDOWS
#
#  if !defined(TINY_DECLSPEC_NORETURN)
#
#    define TINY_DECLSPEC_NORETURN __declspec(noreturn)
#
#  endif // TINY_DECLSPEC_NORETURN
#
#  if !defined(TINY_DECLSPEC_DEPRECATED)
#
#    define TINY_DECLSPEC_DEPRECATED __declspec(deprecated)
#
#  endif // TINY_DECLSPEC_DEPRECATED
#
#else // TINY_PLATFORM != TINY_PLATFORM_WINDOWS
#
#  define TINY_DECLSPEC_NORETURN
#  define TINY_DECLSPEC_DEPRECATED
#
#endif // TINY_PLATFORM


/**
 *
 * noreturn
 *
 * 该属性通知编译器函数从不返回值
 *
 *
 * deprecated
 *
 * 该属性标识预期在程序的新的版本或未来版本中不再支持
 *
 *
 * format (archetype, string-index, first-to-check)
 *
 * archetype      : 指定是哪种风格 (printf scanf strftime strfmon)
 * string-index   : 指定传入函数的第几个参数是格式化字符串
 * first-to-check : 指定从函数的第几个参数开始按上述规则进行检查
 *
 * 该属性可以使编译器检查函数声明和函数实际调用参数之间的格式化字符串是否匹配
 *
 */
#if TINY_COMPILER == TINY_COMPILER_GNU
#
#  define TINY_ATTR_NORETURN __attribute__((noreturn))
#  define TINY_ATTR_DEPRECATED __attribute__((deprecated))
#  define TINY_ATTR_PRINTF(index, check) __attribute__((format(printf, index, check)))
#
#else // TINY_COMPILER != TINY_COMPILER_GNU
#
#  define TINY_ATTR_NORETURN
#  define TINY_ATTR_DEPRECATED
#  define TINY_ATTR_PRINTF(index, check)
#
#endif // TINY_COMPILER == TINY_COMPILER_GNU


/**
 *
 * link符号选项
 *
 */
#ifdef TINY_API_USE_DYNAMIC_LINKING
#
#  if TINY_COMPILER == TINY_COMPILER_MICROSOFT
#
#    define TINY_API_EXPORT __declspec(dllexport)
#    define TINY_API_IMPORT __declspec(dllimport)
#
#  elif TINY_COMPILER == TINY_COMPILER_GNU
#
#    define TINY_API_EXPORT __attribute__((visibility("default")))
#    define TINY_API_IMPORT
#
#  else
#
#    error compiler not supported!
#
#  endif
#
#else
#
#  define TINY_API_EXPORT
#  define TINY_API_IMPORT
#
#endif


/**
 *
 * 符号操作
 *
 */
#ifdef TINY_API_EXPORT_COMMON
#
#  define TINY_API TINY_API_EXPORT
#
#else
#
#  define TINY_API TINY_API_IMPORT
#
#endif


/**
 *
 * sleep操作
 *
 */
#define TINY_SLEEP_H(n)		std::this_thread::sleep_for(std::chrono::hours(n));
#define TINY_SLEEP_M(n)		std::this_thread::sleep_for(std::chrono::minutes(n));
#define TINY_SLEEP_S(n)		std::this_thread::sleep_for(std::chrono::seconds(n));
#define TINY_SLEEP_NS(n)	std::this_thread::sleep_for(std::chrono::nanoseconds(n));
#define TINY_SLEEP_MS(n)	std::this_thread::sleep_for(std::chrono::milliseconds(n));
#define TINY_SLEEP_US(n)	std::this_thread::sleep_for(std::chrono::microseconds(n));


using ByteSet = std::set<Byte>;
using ByteVector = std::vector<Byte>;
using ByteUnorderedSet = std::unordered_set<Byte>;

using StringSet = std::set<std::string>;
using StringVector = std::vector<std::string>;
using StringUnorderedSet = std::unordered_set<std::string>;

using BoostDate = boost::gregorian::date;
using BoostPtime = boost::posix_time::ptime;
using BoostTimeDuration = boost::posix_time::time_duration;

using SteadyClock = std::chrono::steady_clock;
using SteadyClockDuration = std::chrono::steady_clock::duration;
using SteadyClockTimesPoint = std::chrono::steady_clock::time_point;

using SystemClock = std::chrono::system_clock;
using SystemClockDuration = std::chrono::system_clock::duration;
using SystemClockTimesPoint = std::chrono::system_clock::time_point;


template <typename TypeT>
struct HashFunc
{
	size_t operator() (const TypeT & src) const
	{
		return (size_t)src;
	}
};


template <typename TypeT>
struct CompareFunc
{
	bool operator() (const TypeT & src, const TypeT & dst) const
	{
		return src == dst;
	}
};


#endif // __TINY_CORE__COMMON__COMMON__H__
