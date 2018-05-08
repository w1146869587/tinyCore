/**
 *
 *  作者: hm
 *
 *  说明: 主函数
 *
 */


#include "main.h"

#include <gperftools/profiler.h>


void ParseOption(int argc, char const * argv[])
{
	TINY_OPTION_DEFINE("sync", "sync test", "Sync options")
	TINY_OPTION_DEFINE("async", "async test", "Async options")

	TINY_OPTION_DEFINE_ARG("count",  "log write count", "1000000")
	TINY_OPTION_DEFINE_ARG("thread", "log thread count", "10")

	TINY_OPTION_DEFINE_VERSION("2018-05-08")

	TINY_OPTION_PARSE(argc, argv);
}

void StartApp()
{
	if (TINY_OPTION_HAS("sync"))
	{
		Example::Sync(TINY_STR_TO_DIGITAL(std::size_t, TINY_OPTION_GET("count")));
	}
	else if (TINY_OPTION_HAS("async"))
	{
		Example::Async(TINY_STR_TO_DIGITAL(std::size_t, TINY_OPTION_GET("count")),
					   TINY_STR_TO_DIGITAL(std::size_t, TINY_OPTION_GET("thread")));
	}
	else
	{
		Example::Test(TINY_STR_TO_DIGITAL(std::size_t, TINY_OPTION_GET("count")),
					  TINY_STR_TO_DIGITAL(std::size_t, TINY_OPTION_GET("thread")));
	}
}

void CloseApp(int32_t signalNo = 0)
{
	ProfilerStop();
}

void KillApp(int32_t signalNo = 0)
{
	CloseApp(signalNo);

	exit(0);
}

int main(int argc, char const * argv[])
{
	ParseOption(argc, argv);

	if (TINY_PROCESS_IS_RUNNING())
	{
		return 0;
	}

	TINY_SIGNAL_REGISTER_IGNORE()
	TINY_SIGNAL_REGISTER_TERMINATE(KillApp)
	TINY_SIGNAL_REGISTER_STACK_FRAME(tinyCore::debug::Backtrace::Trace)

	ProfilerStart("my.prof");

	StartApp();
	CloseApp();

	return 0;
}
