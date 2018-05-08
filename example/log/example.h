#ifndef __EXAMPLE__LOG__EXAMPLE__H__
#define __EXAMPLE__LOG__EXAMPLE__H__


#include <tinyCore/tinyCore.h>


using namespace std::chrono;
using namespace tinyCore::log;


class Example
{
public:
	static void Test(const std::size_t msgCount = 1000000, const std::size_t threadCount = 10)
	{
		Sync(msgCount);
		Async(msgCount, threadCount);
	}

	static void Sync(const std::size_t msgCount = 1000000)
	{
		std::cout << std::endl;
		std::cout << "*******************************************************************************" << std::endl;
		std::cout << "Single thread, " << msgCount << " iterations" << std::endl;
		std::cout << "*******************************************************************************" << std::endl;
		std::cout << std::endl;

		auto null     = RegistryLogger::NullLoggerSync("null_sync");
		auto file     = RegistryLogger::FileLoggerSync("file_sync", "logs/file_sync.log", true);
		auto daily    = RegistryLogger::DailyFileLoggerSync("daily_sync", "logs/daily_sync.log");
		auto stream   = RegistryLogger::OStreamLoggerSync("stream_sync", std::make_shared<std::ostringstream>());
		auto rotating = RegistryLogger::RotatingFileLoggerSync("rotating_sync", "logs/rotating_sync.log",  30 * TINY_MB, 5);

		TestSync("null sync logger", null, msgCount);

		RegistryLogger::Remove("null sync logger");

		TestSync("file sync logger", file, msgCount);

		RegistryLogger::Remove("file sync logger");

		TestSync("daily sync logger", daily, msgCount);

		RegistryLogger::Remove("daily sync logger");

		TestSync("stream sync logger", stream, msgCount);

		RegistryLogger::Remove("stream sync logger");

		TestSync("rotating sync logger", rotating, msgCount);

		RegistryLogger::Remove("rotating sync logger");
	}

	static void Async(const std::size_t msgCount = 1000000, const std::size_t threadCount = 10)
	{
		std::cout << std::endl;
		std::cout << "*******************************************************************************" << std::endl;
		std::cout << threadCount << " threads sharing same logger, " << msgCount << " iterations" << std::endl;
		std::cout << "*******************************************************************************" << std::endl;
		std::cout << std::endl;

		auto null     = RegistryLogger::NullLoggerAsync("null_async");
		auto file     = RegistryLogger::FileLoggerAsync("file_async", "logs/file_async.log", true);
		auto daily    = RegistryLogger::DailyFileLoggerAsync("daily_async", "logs/daily_async.log");
		auto stream   = RegistryLogger::OStreamLoggerAsync("stream_async", std::make_shared<std::ostringstream>());
		auto rotating = RegistryLogger::RotatingFileLoggerAsync("rotating_async", "logs/rotating_async.log",  30 * TINY_MB, 5);

		TestAsync("null async logger", null, msgCount, threadCount);

		RegistryLogger::Remove("null async logger");

		TestAsync("file async logger", file, msgCount, threadCount);

		RegistryLogger::Remove("file async logger");

		TestAsync("daily async logger", daily, msgCount, threadCount);

		RegistryLogger::Remove("daily async logger");

		TestAsync("stream async logger", stream, msgCount, threadCount);

		RegistryLogger::Remove("stream async logger");

		TestAsync("rotating async logger", rotating, msgCount, threadCount);

		RegistryLogger::Remove("rotating async logger");
	}

protected:
	static void TestSync(const char * description, const std::shared_ptr<ILogger> & logger, const std::size_t count)
	{
		std::cout << description << "..." << std::endl;

		////////////////////////////////////////////////////////////////////////////////////////////////////

		std::vector<std::size_t> result;

		////////////////////////////////////////////////////////////////////////////////////////////////////

		auto start = TINY_TIME_POINT();

		for (std::size_t i = 0; i < count; ++i)
		{
			auto start_time = TINY_TIME_POINT();

			logger->Info("Hello logger: msg number [thread={} id={}]", 0, i);

			auto stop_time = TINY_TIME_POINT();

			result.emplace_back(TINY_TIME_MICROSECONDS(stop_time - start_time));
		}

		auto stop = TINY_TIME_POINT();

		////////////////////////////////////////////////////////////////////////////////////////////////////

		auto all = TINY_TIME_MICROSECONDS(stop - start);
		auto max = *std::max_element(result.begin(), result.end());
		auto min = *std::min_element(result.begin(), result.end());
		auto avg = accumulate(begin(result), end(result), 0.0, std::plus<std::time_t>()) / result.size();

		std::cout << "max  : " << TINY_STR_TO_LOCAL(max) << " us" << std::endl;
		std::cout << "min  : " << TINY_STR_TO_LOCAL(min) << " us" << std::endl;
		std::cout << "avg  : " << TINY_STR_TO_LOCAL(avg) << " us" << std::endl;
		std::cout << "all  : " << TINY_STR_TO_LOCAL(all) << " us" << std::endl;
		std::cout << "rate : " << TINY_STR_TO_LOCAL(count / TINY_TIME_DOUBLE(stop - start)) << "/sec" << std::endl << std::endl;
	}

	static void TestAsync(const char * description, const std::shared_ptr<ILogger> & logger, const std::size_t count,
																							 const std::size_t threadCount)
	{
		std::cout << description << "..." << std::endl;

		////////////////////////////////////////////////////////////////////////////////////////////////////

		std::vector<std::size_t> result;
		std::vector<std::thread> threads;

		std::atomic<std::size_t> counter{ 0 };

		std::map<std::size_t, std::vector<std::time_t>> threadsResult;

		////////////////////////////////////////////////////////////////////////////////////////////////////

		auto start = TINY_TIME_POINT();

		for (std::size_t i = 0; i < threadCount; ++i)
		{
			threads.emplace_back
			(
				std::thread
				(
					[&](const std::size_t index, std::vector<std::time_t> & result)
					{
						while (true)
						{
							std::size_t id = ++counter;

							if (id > count)
							{
								break;
							}

							auto start_time = TINY_TIME_POINT();

							logger->Info("Hello logger: msg number [thread={} id={}]", index, id);

							auto stop_time = TINY_TIME_POINT();

							result.emplace_back(TINY_TIME_MICROSECONDS(stop_time - start_time));
						}
					}, i, std::ref(threadsResult[i])
				)
			);
		}

		for (auto &t : threads)
		{
			t.join();
		};

		auto stop = TINY_TIME_POINT();

		////////////////////////////////////////////////////////////////////////////////////////////////////

		for (auto &iter : threadsResult)
		{
			result.insert(result.end(), iter.second.begin(), iter.second.end());
		}

		auto all = TINY_TIME_MICROSECONDS(stop - start);
		auto max = *std::max_element(result.begin(), result.end());
		auto min = *std::min_element(result.begin(), result.end());
		auto avg = accumulate(begin(result), end(result), 0.0, std::plus<std::time_t>()) / result.size();

		std::cout << "max  : " << TINY_STR_TO_LOCAL(max) << " us" << std::endl;
		std::cout << "min  : " << TINY_STR_TO_LOCAL(min) << " us" << std::endl;
		std::cout << "avg  : " << TINY_STR_TO_LOCAL(avg) << " us" << std::endl;
		std::cout << "all  : " << TINY_STR_TO_LOCAL(all) << " us" << std::endl;
		std::cout << "rate : " << TINY_STR_TO_LOCAL(count / TINY_TIME_DOUBLE(stop - start)) << "/sec" << std::endl << std::endl;
	}
};


#endif // __EXAMPLE__LOG__EXAMPLE__H__
