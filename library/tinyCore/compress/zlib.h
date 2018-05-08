#ifndef __TINY_CORE__COMPRESS__ZLIB__H__
#define __TINY_CORE__COMPRESS__ZLIB__H__


/**
 *
 *  作者: hm
 *
 *  说明: zlib解压缩
 *
 */


#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace compress
	{
		#define ARRANGE_OUTPUT_BUFFER(zst, buffer, length)			\
		{															\
			std::size_t occupied = 0;								\
																	\
			if (buffer.empty())										\
			{														\
				occupied = 0;										\
																	\
				buffer.resize(length);								\
			}														\
			else													\
			{														\
				occupied = zst.next_out - (Bytef *)buffer.data();	\
																	\
				if (occupied == length)								\
				{													\
					length <<= 1;									\
																	\
					buffer.resize(length);							\
				}													\
			}														\
																	\
			zst.avail_out = (uInt)(length - occupied);				\
			zst.next_out  = (Bytef *)buffer.data() + occupied;		\
		}

		#define ARRANGE_OUTPUT_BUFFER_LIMIT(zst, buffer, length, maxLength)		\
		{																		\
			std::size_t occupied = 0;											\
																				\
			if (buffer.empty())													\
			{																	\
				occupied = 0;													\
																				\
				buffer.resize(length);											\
			}																	\
			else																\
			{																	\
				occupied = zst.next_out - (Bytef *)buffer.data();				\
																				\
				if (occupied == length)											\
				{																\
					if (maxLength == 0)											\
					{															\
						length <<= 1;											\
																				\
						buffer.resize(length);									\
					}															\
					else														\
					{															\
						size_t newLength;										\
 																				\
						if (length == maxLength)								\
						{														\
							break;												\
						}														\
																				\
						if (length <= (maxLength >> 1))							\
						{														\
							newLength = length << 1;							\
						}														\
						else													\
						{														\
							newLength = maxLength;								\
						}														\
																				\
						length = newLength;										\
																				\
						buffer.resize(newLength);								\
					}															\
				}																\
			}																	\
																				\
			zst.avail_out = (uInt)(length - occupied);							\
			zst.next_out  = (Bytef *)buffer.data() + occupied;					\
		}

		class ZLib
		{
		public:
			static uLong Crc32(const char * data, const uLong value = 0)
			{
				TINY_ASSERT(data, "data is nullptr")

				return Crc32((const Byte *)data, strlen(data), value);
			}

			static uLong Crc32(const ByteVector & data, const uLong value = 0)
			{
				return Crc32(data.data(), data.size(), value);
			}

			static uLong Crc32(const std::string & data, const uLong value = 0)
			{
				return Crc32((const Byte *)data.data(), data.size(), value);
			}

			static uLong Crc32(const Byte * data, const std::size_t len, const uLong value = 0)
			{
				TINY_ASSERT(data, "data is nullptr")

				return ::crc32(value, data, (uInt)len);
			}

			static uLong Adler32(const char * data, const uLong value = 1)
			{
				TINY_ASSERT(data, "data is nullptr")

				return Adler32((const Byte *)data, strlen(data), value);
			}

			static uLong Adler32(const ByteVector & data, const uLong value = 1)
			{
				return Adler32(data.data(), data.size(), value);
			}

			static uLong Adler32(const std::string & data, const uLong value = 1)
			{
				return Adler32((const Byte *)data.data(), data.size(), value);
			}

			static uLong Adler32(const Byte * data, const std::size_t len, const uLong value = 1)
			{
				TINY_ASSERT(data, "data is nullptr")

				return ::adler32(value, data, (uInt)len);
			}

			static void HandleError(const z_stream & zst, const int32_t error, const char * msg)
			{
				TINY_ASSERT(msg, "msg is nullptr")

				const char * errorMsg = nullptr;

				if (error == Z_VERSION_ERROR)
				{
					errorMsg = "library version mismatch";
				}
				else
				{
					errorMsg = zst.msg;
				}

				if (errorMsg == nullptr)
				{
					switch (error)
					{
						case Z_BUF_ERROR:
						{
							errorMsg = "incomplete or truncated stream";

							break;
						}

						case Z_STREAM_ERROR:
						{
							errorMsg = "inconsistent stream state";

							break;
						}

						case Z_DATA_ERROR:
						{
							errorMsg = "invalid input data";

							break;
						}

						default:
						{
							break;
						}
					}
				}

				if (errorMsg)
				{
					TINY_THROW_EXCEPTION(debug::ZlibError, TINY_STR_FORMAT("Error {} {}: {}", error, msg, errorMsg));
				}
				else
				{
					TINY_THROW_EXCEPTION(debug::ZlibError, TINY_STR_FORMAT("Error {} {}", error, msg));
				}
			}

			template <typename ValueT>
			static ValueT Compress(const char * data, const int32_t level = ZLIB_DEFAULT_COMPRESSION)
			{
				TINY_ASSERT(data, "data is nullptr")

				ValueT retVal = { };

				Compress(retVal, (const Byte *)data, strlen(data), level);

				return retVal;
			}

			template <typename ValueT>
			static ValueT Compress(const ByteVector & data, const int32_t level = ZLIB_DEFAULT_COMPRESSION)
			{
				ValueT retVal = { };

				Compress(retVal, data.data(), data.size(), level);

				return retVal;
			}

			template <typename ValueT>
			static ValueT Compress(const std::string & data, const int32_t level = ZLIB_DEFAULT_COMPRESSION)
			{
				ValueT retVal = { };

				Compress(retVal, (const Byte *)data.data(), data.size(), level);

				return retVal;
			}

			template <typename ValueT>
			static ValueT Decompress(const char * data, const int32_t wBits = ZLIB_MAX_WBITS)
			{
				TINY_ASSERT(data, "data is nullptr")

				ValueT retVal = { };

				Decompress(retVal, (const Byte *)data, strlen(data), wBits);

				return retVal;
			}

			template <typename ValueT>
			static ValueT Decompress(const ByteVector & data, const int32_t wBits = ZLIB_MAX_WBITS)
			{
				ValueT retVal = { };

				Decompress(retVal, data.data(), data.size(), wBits);

				return retVal;
			}

			template <typename ValueT>
			static ValueT Decompress(const std::string & data, const int32_t wBits = ZLIB_MAX_WBITS)
			{
				ValueT retVal = { };

				Decompress(retVal, (const Byte *)data.data(), data.size(), wBits);

				return retVal;
			}

		protected:
			template <typename ValueT>
			static void Compress(ValueT & retVal, const Byte * data, const std::size_t len, const int32_t level)
			{
				TINY_ASSERT(data, "data is nullptr")

				z_stream zst = { };

				zst.zfree = (free_func)Z_NULL;
				zst.zalloc = (alloc_func)Z_NULL;
				zst.next_in = (Bytef *)data;

				int32_t error = deflateInit(&zst, level);

				if (error != Z_OK)
				{
					if (error == Z_MEM_ERROR)
					{
						TINY_THROW_EXCEPTION(debug::MemoryError, "Out of memory while compressing data");
					}
					else if (error == Z_STREAM_ERROR)
					{
						TINY_THROW_EXCEPTION(debug::ZlibError, "Bad compression level");
					}
					else
					{
						deflateEnd(&zst);

						HandleError(zst, error, "while compressing data");
					}
				}

				zst.avail_in = (uInt)len;

				std::size_t outBufferLen = (std::size_t)ZLIB_DEFAULT_ALLOC;

				do
				{
					ARRANGE_OUTPUT_BUFFER(zst, retVal, outBufferLen);

					error = deflate(&zst, ZLIB_FINISH);

					if (error == Z_STREAM_ERROR)
					{
						deflateEnd(&zst);

						TINY_THROW_EXCEPTION(debug::ZlibError, "while compressing data");
					}
				} while (zst.avail_out == 0);

				TINY_ASSERT(zst.avail_in == 0);
				TINY_ASSERT(error == Z_STREAM_END);

				error = deflateEnd(&zst);

				if (error != Z_OK)
				{
					HandleError(zst, error, "while finishing compression");
				}

				retVal.resize(zst.next_out - (Bytef *)retVal.data());
			}

			template <typename ValueT>
			static void Decompress(ValueT & retVal, const Byte * data, const std::size_t len, const int32_t wBits)
			{
				TINY_ASSERT(data, "data is nullptr")

				z_stream zst = { };

				zst.zfree = (free_func)Z_NULL;
				zst.zalloc = (alloc_func)Z_NULL;
				zst.next_in  = (Bytef *)data;
				zst.avail_in = 0;

				int32_t error = inflateInit2(&zst, wBits);

				if (error != Z_OK)
				{
					if (error == Z_MEM_ERROR)
					{
						TINY_THROW_EXCEPTION(debug::MemoryError, "Out of memory while decompressing data");
					}
					else
					{
						inflateEnd(&zst);

						HandleError(zst, error, "while preparing to decompress data");
					}
				}

				zst.avail_in = (uInt)len;

				std::size_t outBufferLen = (std::size_t)ZLIB_DEFAULT_ALLOC;

				do
				{
					ARRANGE_OUTPUT_BUFFER(zst, retVal, outBufferLen);

					error = inflate(&zst, ZLIB_FINISH);

					if (error != Z_OK && error != Z_BUF_ERROR && error != Z_STREAM_END)
					{
						inflateEnd(&zst);

						if (error == Z_MEM_ERROR)
						{
							TINY_THROW_EXCEPTION(debug::MemoryError, "Out of memory while decompressing data");
						}
						else
						{
							HandleError(zst, error, "while decompressing data");
						}
					}
				} while (zst.avail_out == 0);

				if (error != Z_STREAM_END)
				{
					inflateEnd(&zst);

					HandleError(zst, error, "while decompressing data");
				}

				error = inflateEnd(&zst);

				if (error != Z_OK)
				{
					HandleError(zst, error, "while finishing data decompression");
				}

				retVal.resize(zst.next_out - (Bytef *)retVal.data());
			}

		public:
			static const int ZLIB_FINISH = Z_FINISH;
			static const int ZLIB_DEFLATED = 8;
			static const int ZLIB_NO_FLUSH = Z_NO_FLUSH;
			static const int ZLIB_FILTERED = Z_FILTERED;
			static const int ZLIB_MAX_WBITS = MAX_WBITS;
			static const int ZLIB_BEST_SPEED = Z_BEST_SPEED;
			static const int ZLIB_SYNC_FLUSH = Z_SYNC_FLUSH;
			static const int ZLIB_FULL_FLUSH = Z_FULL_FLUSH;
			static const int ZLIB_HUFFMAN_ONLY = Z_HUFFMAN_ONLY;
			static const int ZLIB_DEFAULT_ALLOC = 16 * TINY_KB;
			static const int ZLIB_BEST_COMPRESSION = Z_BEST_COMPRESSION;
			static const int ZLIB_DEFAULT_STRATEGY = Z_DEFAULT_STRATEGY;
			static const int ZLIB_DEFAULT_COMPRESSION = Z_DEFAULT_COMPRESSION;

#if MAX_MEM_LEVEL >= 8
			static const int ZLIB_DEF_MEM_LEVEL = 8;
#else
			static const int ZLIB_DEF_MEM_LEVEL = MAX_MEM_LEVEL;
#endif
		};

		class ZLibCompress
		{
		public:
			explicit ZLibCompress(const int32_t level = ZLib::ZLIB_DEFAULT_COMPRESSION,
								  const int32_t method = ZLib::ZLIB_DEFLATED,
								  const int32_t wBits = ZLib::ZLIB_MAX_WBITS,
								  const int32_t memLevel = ZLib::ZLIB_DEF_MEM_LEVEL,
								  const int32_t strategy = ZLib::ZLIB_DEFAULT_STRATEGY) :
					_level(level),
					_wBits(wBits),
					_method(method),
					_memLevel(memLevel),
					_strategy(strategy)
			{
				_zst.zfree = (free_func)Z_NULL;
				_zst.zalloc = (alloc_func)Z_NULL;
				_zst.next_in = nullptr;
				_zst.avail_in = 0;
			}

			ZLibCompress(ZLibCompress && rhs) noexcept : _isInited(rhs._isInited),
														 _level(rhs._level),
														 _wBits(rhs._wBits),
														 _method(rhs._method),
														 _memLevel(rhs._memLevel),
														 _strategy(rhs._strategy)
			{
				_zst = rhs._zst;

				if (rhs._isInited)
				{
					rhs._isInited = false;
					rhs._zst.zfree = (free_func)Z_NULL;
					rhs._zst.zalloc = (alloc_func)Z_NULL;
					rhs._zst.next_in = nullptr;
					rhs._zst.avail_in = 0;
				}
			}

			ZLibCompress(const ZLibCompress & rhs) : _isInited(rhs._isInited),
													 _level(rhs._level),
													 _wBits(rhs._wBits),
													 _method(rhs._method),
													 _memLevel(rhs._memLevel),
													 _strategy(rhs._strategy)
			{
				if (_isInited)
				{
					int32_t error = deflateCopy(&_zst, const_cast<z_stream *>(&rhs._zst));

					if (error != Z_OK)
					{
						_isInited = false;

#ifdef DEBUG

						if (error == Z_STREAM_ERROR)
						{
							TINY_THROW_EXCEPTION(debug::ValueError, "Inconsistent stream state");
						}
						else if (error == Z_MEM_ERROR)
						{
							TINY_THROW_EXCEPTION(debug::MemoryError, "Can't allocate memory for compression object");
						}
						else
						{
							ZLib::HandleError(rhs._zst, error, "while copying compression object");
						}

#endif
					}
				}
				else
				{
					_zst.zfree = (free_func)Z_NULL;
					_zst.zalloc = (alloc_func)Z_NULL;
					_zst.next_in = nullptr;
					_zst.avail_in = 0;
				}
			}

			~ZLibCompress()
			{
				Clear();
			}

			ZLibCompress & operator=(ZLibCompress && rhs) noexcept
			{
				Clear();

				_zst = rhs._zst;
				_level = rhs._level;
				_wBits = rhs._wBits;
				_method = rhs._method;
				_memLevel = rhs._memLevel;
				_strategy = rhs._strategy;
				_isInited = rhs._isInited;

				if (rhs._isInited)
				{
					rhs._isInited = false;
					rhs._zst.zfree = (free_func)Z_NULL;
					rhs._zst.zalloc = (alloc_func)Z_NULL;
					rhs._zst.next_in = nullptr;
					rhs._zst.avail_in = 0;
				}

				return *this;
			}

			ZLibCompress & operator=(const ZLibCompress & rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}

				Clear();

				_level = rhs._level;
				_wBits = rhs._wBits;
				_method = rhs._method;
				_memLevel = rhs._memLevel;
				_strategy = rhs._strategy;
				_isInited = rhs._isInited;

				if (_isInited)
				{
					int32_t error = deflateCopy(&_zst, const_cast<z_stream *>(&rhs._zst));

					if (error != Z_OK)
					{
						_isInited = false;

						if (error == Z_STREAM_ERROR)
						{
							TINY_THROW_EXCEPTION(debug::ValueError, "Inconsistent stream state");
						}
						else if (error == Z_MEM_ERROR)
						{
							TINY_THROW_EXCEPTION(debug::MemoryError, "Can't allocate memory for compression object");
						}
						else
						{
							ZLib::HandleError(rhs._zst, error, "while copying compression object");
						}
					}
				}

				return *this;
			}

			template <typename ValueT>
			ValueT Compress(const char * data)
			{
				TINY_ASSERT(data, "data is nullptr")

				ValueT retVal = { };

				Compress(retVal, (const Byte *)data, strlen(data));

				return retVal;
			}

			template <typename ValueT>
			ValueT Compress(const ByteVector & data)
			{
				ValueT retVal = { };

				Compress(retVal, data.data(), data.size());

				return retVal;
			}

			template <typename ValueT>
			ValueT Compress(const std::string & data)
			{
				ValueT retVal = { };

				Compress(retVal, (const Byte *)data.data(), data.size());

				return retVal;
			}

			template <typename ValueT>
			ValueT Compress(const Byte * data, const std::size_t len)
			{
				TINY_ASSERT(data, "data is nullptr")

				ValueT retVal = { };

				Compress(retVal, data, len);

				return retVal;
			}

			template <typename ValueT>
			ValueT Flush(const int32_t flushMode = ZLib::ZLIB_FINISH)
			{
				ValueT retVal = { };

				Flush(retVal, flushMode);

				return retVal;
			}

		protected:
			void Clear()
			{
				if (_isInited)
				{
					deflateEnd(&_zst);

					_zst.zfree = (free_func)Z_NULL;
					_zst.zalloc = (alloc_func)Z_NULL;
					_zst.next_in = nullptr;
					_zst.avail_in = 0;

					_isInited = false;
				}
			}

			void Initialize()
			{
				if (!_isInited)
				{
					int32_t error = deflateInit2(&_zst, _level, _method, _wBits, _memLevel, _strategy);

					if (error == Z_OK)
					{
						_isInited = true;
					}
					else if (error == Z_MEM_ERROR)
					{
						TINY_THROW_EXCEPTION(debug::MemoryError, "Can't allocate memory for compression object");
					}
					else if (error == Z_STREAM_ERROR)
					{
						TINY_THROW_EXCEPTION(debug::ValueError, "Invalid initialization option");
					}
					else
					{
						ZLib::HandleError(_zst, error, "while creating compression object");
					}
				}
			}

			template <typename ValueT>
			void Flush(ValueT & retVal, const int32_t flushMode)
			{
				Initialize();

				if (flushMode == ZLib::ZLIB_NO_FLUSH)
				{
					return;
				}

				int32_t error;

				std::size_t outBufferLen = ZLib::ZLIB_DEFAULT_ALLOC;

				_zst.avail_in = 0;

				do
				{
					ARRANGE_OUTPUT_BUFFER(_zst, retVal, outBufferLen);

					error = deflate(&_zst, flushMode);

					if (error == Z_STREAM_ERROR)
					{
						ZLib::HandleError(_zst, error, "while flushing");
					}
				} while(_zst.avail_out == 0);

				TINY_ASSERT(_zst.avail_in == 0);

				if (error == Z_STREAM_END && flushMode == ZLib::ZLIB_FINISH)
				{
					_isInited = false;

					error = deflateEnd(&_zst);

					if (error != Z_OK)
					{
						ZLib::HandleError(_zst, error, "from deflateEnd()");
					}
				}
				else if (error != Z_OK && error != Z_BUF_ERROR)
				{
					ZLib::HandleError(_zst, error, "while flushing");
				}

				retVal.resize(_zst.next_out - (Bytef *)retVal.data());
			}

			template <typename ValueT>
			void Compress(ValueT & retVal, const Byte * data, const std::size_t len)
			{
				TINY_ASSERT(data, "data is nullptr")

				Initialize();

				int32_t error;

				std::size_t outBufferLen = ZLib::ZLIB_DEFAULT_ALLOC;

				_zst.next_in  = (Bytef *)data;
				_zst.avail_in = (uInt)len;

				do
				{
					ARRANGE_OUTPUT_BUFFER(_zst, retVal, outBufferLen);

					error = deflate(&_zst, ZLib::ZLIB_NO_FLUSH);

					if (error == Z_STREAM_ERROR)
					{
						ZLib::HandleError(_zst, error, "while compressing data");
					}
				} while (_zst.avail_out == 0);

				TINY_ASSERT(_zst.avail_in == 0);

				retVal.resize(_zst.next_out - (Bytef *)retVal.data());
			}

		protected:
			bool _isInited{ false };

			int32_t _level{ ZLib::ZLIB_DEFAULT_COMPRESSION };
			int32_t _wBits{ ZLib::ZLIB_MAX_WBITS };
			int32_t _method{ ZLib::ZLIB_DEFLATED };
			int32_t _memLevel{ ZLib::ZLIB_DEF_MEM_LEVEL };
			int32_t _strategy{ ZLib::ZLIB_DEFAULT_STRATEGY };

			z_stream _zst{ };
		};

		class ZLibDecompress
		{
		public:
			explicit ZLibDecompress(const int32_t wBits = ZLib::ZLIB_MAX_WBITS) : _wBits(wBits)

			{
				_zst.zfree = (free_func)Z_NULL;
				_zst.zalloc = (alloc_func)Z_NULL;
				_zst.next_in = nullptr;
				_zst.avail_in = 0;
			}

			ZLibDecompress(ZLibDecompress && rhs) noexcept : _isInited(rhs._isInited),
															 _wBits(rhs._wBits),
															 _unusedData(std::move(rhs._unusedData)),
															 _unconsumedTail(std::move(rhs._unconsumedTail))
			{
				_zst = rhs._zst;

				if (rhs._isInited)
				{
					rhs._isInited = false;
					rhs._zst.zfree = (free_func)Z_NULL;
					rhs._zst.zalloc = (alloc_func)Z_NULL;
					rhs._zst.next_in = nullptr;
					rhs._zst.avail_in = 0;
				}
			}

			ZLibDecompress(const ZLibDecompress & rhs) : _isInited(rhs._isInited),
														 _wBits(rhs._wBits),
														 _unusedData(rhs._unusedData),
														 _unconsumedTail(rhs._unconsumedTail)
			{
				if (_isInited)
				{
					int32_t error = inflateCopy(&_zst, const_cast<z_stream *>(&rhs._zst));

					if (error != Z_OK)
					{
						_isInited = false;

#ifdef DEBUG

						if (error == Z_STREAM_ERROR)
						{
							TINY_THROW_EXCEPTION(debug::ValueError, "Inconsistent stream state");
						}
						else if (error == Z_MEM_ERROR)
						{
							TINY_THROW_EXCEPTION(debug::MemoryError, "Can't allocate memory for decompression object");
						}
						else
						{
							ZLib::HandleError(rhs._zst, error, "while copying decompression object");
						}

#endif
					}
				}
				else
				{
					_zst.zfree = (free_func)Z_NULL;
					_zst.zalloc = (alloc_func)Z_NULL;
					_zst.next_in = nullptr;
					_zst.avail_in = 0;
				}
			}

			~ZLibDecompress()
			{
				Clear();
			}

			const ByteVector & GetUnusedData() const
			{
				return _unusedData;
			}

			const ByteVector & GetUnconsumedTail() const
			{
				return _unconsumedTail;
			}

			ZLibDecompress & operator=(ZLibDecompress && rhs) noexcept
			{
				Clear();

				_zst = rhs._zst;
				_wBits = rhs._wBits;
				_isInited = rhs._isInited;
				_unusedData = std::move(rhs._unusedData);
				_unconsumedTail = std::move(rhs._unconsumedTail);

				if (rhs._isInited)
				{
					rhs._isInited = false;
					rhs._zst.zfree = (free_func)Z_NULL;
					rhs._zst.zalloc = (alloc_func)Z_NULL;
					rhs._zst.next_in = nullptr;
					rhs._zst.avail_in = 0;
				}

				return *this;
			}

			ZLibDecompress & operator=(const ZLibDecompress & rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}

				Clear();

				_wBits = rhs._wBits;
				_isInited = rhs._isInited;
				_unusedData = rhs._unusedData;
				_unconsumedTail = rhs._unconsumedTail;

				if (_isInited)
				{
					int32_t error = inflateCopy(&_zst, const_cast<z_stream *>(&rhs._zst));

					if (error != Z_OK)
					{
						_isInited = false;

						if (error == Z_STREAM_ERROR)
						{
							TINY_THROW_EXCEPTION(debug::ValueError, "Inconsistent stream state");
						}
						else if (error == Z_MEM_ERROR)
						{
							TINY_THROW_EXCEPTION(debug::MemoryError, "Can't allocate memory for decompression object");
						}
						else
						{
							ZLib::HandleError(rhs._zst, error, "while copying decompression object");
						}
					}
				}

				return *this;
			}

			template <typename ValueT>
			ValueT Decompress(const char * data, const std::size_t maxLength = 0)
			{
				TINY_ASSERT(data, "data is nullptr")

				ValueT retVal = { };

				Decompress(retVal, (const Byte *)data, strlen(data), maxLength);

				return retVal;
			}

			template <typename ValueT>
			ValueT Decompress(const ByteVector & data, const std::size_t maxLength = 0)
			{
				ValueT retVal = { };

				Decompress(retVal, data.data(), data.size(), maxLength);

				return retVal;
			}

			template <typename ValueT>
			ValueT Decompress(const std::string & data, const std::size_t maxLength = 0)
			{
				ValueT retVal = { };

				Decompress(retVal, (const Byte *)data.data(), data.size(), maxLength);

				return retVal;
			}

			template <typename ValueT>
			ValueT Decompress(const Byte * data, const std::size_t len, const std::size_t maxLength = 0)
			{
				TINY_ASSERT(data, "data is nullptr")

				ValueT retVal = { };

				Decompress(retVal, data, len, maxLength);

				return retVal;
			}

			template <typename ValueT>
			ValueT Flush()
			{
				ValueT retVal = { };

				Flush(retVal);

				return retVal;
			}

		protected:
			void Clear()
			{
				if (_isInited)
				{
					inflateEnd(&_zst);

					_isInited = false;
					_zst.zfree = (free_func)Z_NULL;
					_zst.zalloc = (alloc_func)Z_NULL;
					_zst.next_in = nullptr;
					_zst.avail_in = 0;
				}
			}

			void Initialize()
			{
				if (!_isInited)
				{
					int32_t error = inflateInit2(&_zst, _wBits);

					if (error == Z_OK)
					{
						_isInited = true;
					}
					else if (error == Z_STREAM_ERROR)
					{
						TINY_THROW_EXCEPTION(debug::ValueError, "Invalid initialization option");
					}
					else if (error == Z_MEM_ERROR)
					{
						TINY_THROW_EXCEPTION(debug::MemoryError, "Can't allocate memory for decompression object");
					}
					else
					{
						ZLib::HandleError(_zst, error, "while creating decompression object");
					}
				}
			}

			void SaveUnconsumedInput(const Byte * data, const std::size_t len, const int32_t error)
			{
				TINY_ASSERT(data, "data is nullptr")

				if (error == Z_STREAM_END)
				{
					if (_zst.avail_in > 0)
					{
						std::size_t leftSize = data + len - _zst.next_in;;

						_unusedData.insert(_unusedData.end(), _zst.next_in, _zst.next_in + leftSize);

						_zst.avail_in = 0;
					}
				}

				if (_zst.avail_in > 0 || !_unconsumedTail.empty())
				{
					std::size_t leftSize = data + len - _zst.next_in;

					_unconsumedTail.assign(_zst.next_in, _zst.next_in + leftSize);
				}
			}

			template <typename ValueT>
			void Flush(ValueT & retVal)
			{
				Initialize();

				int32_t error;

				std::size_t outBufferLen = ZLib::ZLIB_DEFAULT_ALLOC;

				_zst.next_in  = _unconsumedTail.data();
				_zst.avail_in = (uInt)_unconsumedTail.size();

				do
				{
					ARRANGE_OUTPUT_BUFFER(_zst, retVal, outBufferLen);

					error = inflate(&_zst, ZLib::ZLIB_FINISH);

					if (error != Z_OK && error != Z_BUF_ERROR && error != Z_STREAM_END)
					{
						break;
					}
				} while (_zst.avail_out == 0);

				SaveUnconsumedInput(_unconsumedTail.data(), _unconsumedTail.size(), error);

				if (error == Z_STREAM_END)
				{
					_isInited = false;

					error = inflateEnd(&_zst);

					if (error != Z_OK)
					{
						ZLib::HandleError(_zst, error, "from inflateEnd()");
					}
				}

				retVal.resize(_zst.next_out - (Bytef *)retVal.data());
			}

			template <typename ValueT>
			void Decompress(ValueT & retVal, const Byte * data, const std::size_t len, const std::size_t maxLength = 0)
			{
				TINY_ASSERT(data, "data is nullptr")

				Initialize();

				int32_t error = Z_OK;

				std::size_t outBufferLen = ZLib::ZLIB_DEFAULT_ALLOC;

				_zst.next_in  = (Bytef *)data;
				_zst.avail_in = (uInt)len;

				do
				{
					ARRANGE_OUTPUT_BUFFER_LIMIT(_zst, retVal, outBufferLen, maxLength);

					error = inflate(&_zst, ZLib::ZLIB_SYNC_FLUSH);

					if (error != Z_OK && error != Z_BUF_ERROR && error != Z_STREAM_END)
					{
						break;
					}
				} while (_zst.avail_out == 0);

				SaveUnconsumedInput(data, len, error);

				if (error != Z_OK && error != Z_BUF_ERROR && error != Z_STREAM_END)
				{
					ZLib::HandleError(_zst, error, "while decompressing");
				}

				retVal.resize(_zst.next_out - (Bytef *)retVal.data());
			}

		protected:
			bool _isInited{ false };

			int32_t _wBits{ ZLib::ZLIB_MAX_WBITS };

			z_stream _zst{ };

			ByteVector _unusedData{ };
			ByteVector _unconsumedTail{ };
		};
	}
}


#define TINY_ZLIB_CRC32(value, ...) tinyCore::compress::ZLib::Crc32(value, ##__VA_ARGS__)
#define TINY_ZLIB_ADLER32(value, ...) tinyCore::compress::ZLib::Adler32(value, ##__VA_ARGS__)
#define TINY_ZLIB_COMPRESS(value, ...) tinyCore::compress::ZLib::Compress(value, ##__VA_ARGS__)
#define TINY_ZLIB_DECOMPRESS(value, ...) tinyCore::compress::ZLib::Decompress(value, ##__VA_ARGS__)


#endif // __TINY_CORE__COMPRESS__ZLIB__H__
