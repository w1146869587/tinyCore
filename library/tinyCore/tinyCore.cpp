/**
 *
 *  作者: hm
 *
 *  说明: tinyCore
 *
 */


#include <tinyCore/tinyCore.h>


namespace tinyCore
{
	namespace id
	{
		GUID::ONLY_ID GUID::_id;
	}

	namespace log
	{
		std::mutex RegistryLogger::_lock;

		std::map<std::string, RegistryLogger::LoggerPtr> RegistryLogger::_loggers;
	}

	namespace system
	{
		std::error_code FileSystem::_code;
	}

	namespace compress
	{
		const std::size_t Gzip::_maxReadChunk;

		const int ZLib::ZLIB_FINISH;
		const int ZLib::ZLIB_DEFLATED;
		const int ZLib::ZLIB_NO_FLUSH;
		const int ZLib::ZLIB_FILTERED;
		const int ZLib::ZLIB_MAX_WBITS;
		const int ZLib::ZLIB_BEST_SPEED;
		const int ZLib::ZLIB_SYNC_FLUSH;
		const int ZLib::ZLIB_FULL_FLUSH;
		const int ZLib::ZLIB_HUFFMAN_ONLY;
		const int ZLib::ZLIB_DEF_MEM_LEVEL;
		const int ZLib::ZLIB_DEFAULT_ALLOC;
		const int ZLib::ZLIB_BEST_COMPRESSION;
		const int ZLib::ZLIB_DEFAULT_STRATEGY;
		const int ZLib::ZLIB_DEFAULT_COMPRESSION;
	}
}
