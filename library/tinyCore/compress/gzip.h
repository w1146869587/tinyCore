#ifndef __TINY_CORE__COMPRESS__GZIP__H__
#define __TINY_CORE__COMPRESS__GZIP__H__


/**
 *
 *  作者: hm
 *
 *  说明: gzip解压缩
 *
 */


#include <boost/endian/conversion.hpp>

#include <tinyCore/compress/zlib.h>
#include <tinyCore/system/fileSystem.h>


namespace tinyCore
{
	namespace compress
	{
		#define TINY_GZIP_FTEXT		1
		#define TINY_GZIP_FHCRC		2
		#define TINY_GZIP_FEXTRA	4
		#define TINY_GZIP_FNAME		8
		#define TINY_GZIP_FCOMMENT	16

		enum class TINY_GZIP_FILE_MODE
		{
			NONE,
			READ,
			WRITE
		};

		class Gzip
		{
		public:
			void InitWithInputFile(const std::string & fileName, std::ios_base::openmode mode = std::fstream::in)
			{
				TINY_ASSERT(mode & std::fstream::in, "Mode not supported")

				CheckMode();

				auto fileStream = std::make_shared<std::fstream>();

				fileStream->open(fileName, mode | std::fstream::binary);

				_mode = TINY_GZIP_FILE_MODE::READ;
				_offset = 0;
				_fileName = fileName;
				_extraSize = 0;
				_extraStart = 0;
				_fileStream = std::move(fileStream);
				_isNewMember = true;
				_minReadSize = 100;
			}

			void InitWithInputStream(std::shared_ptr<std::iostream> fileStream)
			{
				CheckMode();

				_mode = TINY_GZIP_FILE_MODE::READ;
				_offset = 0;
				_fileName = "";
				_extraSize = 0;
				_extraStart = 0;
				_fileStream = std::move(fileStream);
				_isNewMember = true;
				_minReadSize = 100;
			}

			void InitWithOutputFile(const std::string & fileName, std::ios_base::openmode mode = std::fstream::out|std::fstream::trunc, const int32_t compressLevel = 9, const std::time_t * time = nullptr)
			{
				TINY_ASSERT(mode & std::fstream::out, "Mode not supported")

				CheckMode();

				auto fileStream = std::make_shared<std::fstream>();

				fileStream->open(fileName, mode | std::fstream::binary);

				_crc = TINY_ZLIB_CRC32("") & 0xffffffff;
				_mode = TINY_GZIP_FILE_MODE::WRITE;
				_size = 0;
				_offset = 0;
				_fileName = fileName;
				_compress = std::make_unique<ZLibCompress>(compressLevel, ZLib::ZLIB_DEFLATED, -ZLib::ZLIB_MAX_WBITS, ZLib::ZLIB_DEF_MEM_LEVEL, 0);
				_fileStream = std::move(fileStream);

				if (time)
				{
					_time = *time;
				}

				WriteHeader();
			}

			void InitWithOutputStream(std::shared_ptr<std::iostream> fileStream, const int32_t compressLevel = 9, const std::time_t * time = nullptr)
			{
				CheckMode();

				_crc = TINY_ZLIB_CRC32("") & 0xffffffff;
				_mode = TINY_GZIP_FILE_MODE::WRITE;
				_size = 0;
				_offset = 0;
				_fileName = "";
				_compress = std::make_unique<ZLibCompress>(compressLevel, ZLib::ZLIB_DEFLATED, -ZLib::ZLIB_MAX_WBITS, ZLib::ZLIB_DEF_MEM_LEVEL, 0);
				_fileStream = std::move(fileStream);

				if (time)
				{
					_time = *time;
				}

				WriteHeader();
			}

			bool IsOpen() const
			{
				return !IsClosed();
			}


			bool IsClosed() const
			{
				return !_fileStream;
			}

			bool IsSeekable() const
			{
				return true;
			}

			bool IsReadable() const
			{
				return _mode == TINY_GZIP_FILE_MODE::READ;
			}

			bool IsWriteable() const
			{
				return _mode == TINY_GZIP_FILE_MODE::WRITE;
			}

			void Close()
			{
				TINY_ASSERT(_fileStream, "_fileStream is nullptr")

				if (IsClosed())
				{
					return;
				}

				if (_mode == TINY_GZIP_FILE_MODE::WRITE)
				{
					auto crc  = (uint32_t)_crc;
					auto size = (uint32_t)(_size & 0xffffffff);

					TINY_THROW_EXCEPTION_IF(_compress == nullptr, debug::Nullptr, "_compress is nullptr")

					auto tail = _compress->Flush<std::string>();

					_fileStream->write(tail.data(), tail.size());

					boost::endian::native_to_little_inplace(crc);

					_fileStream->write((const char *)&crc, 4);

					boost::endian::native_to_little(size);

					_fileStream->write((const char *)&size, 4);
					_fileStream->flush();

					if (!_fileName.empty())
					{
						auto fileStream = std::dynamic_pointer_cast<std::fstream>(_fileStream);

						if (fileStream)
						{
							fileStream->close();
						}
					}

					_fileStream.reset();
				}
				else if (_mode == TINY_GZIP_FILE_MODE::READ)
				{
					if (!_fileName.empty())
					{
						auto fileStream = std::dynamic_pointer_cast<std::fstream>(_fileStream);

						if (fileStream)
						{
							fileStream->close();
						}
					}

					_fileStream.reset();
				}

				if (!_fileName.empty())
				{
					auto fileStream = std::dynamic_pointer_cast<std::fstream>(_fileStream);

					if (fileStream)
					{
						fileStream->close();
					}
				}

				_fileStream.reset();
			}

			void Rewind()
			{
				TINY_ASSERT(_fileStream, "_fileStream is nullptr")
				TINY_ASSERT(_mode == TINY_GZIP_FILE_MODE::READ, "Can't rewind in write mode")

				_offset = 0;
				_extraSize = 0;
				_extraStart = 0;
				_isNewMember = true;
				_fileStream->seekg(0, std::ios_base::beg);
				_extraBuffer.clear();
			}

			void Flush(const int32_t mode = ZLib::ZLIB_SYNC_FLUSH)
			{
				TINY_ASSERT(_compress, "_compress is nullptr")
				TINY_ASSERT(_fileStream, "_fileStream is nullptr")

				TINY_ASSERT(IsOpen(), "I/O operation on closed file")

				if (_mode == TINY_GZIP_FILE_MODE::WRITE)
				{
					auto tail = _compress->Flush<std::string>(mode);

					_fileStream->write(tail.data(), tail.size());
					_fileStream->flush();
				}
			}

			std::size_t Write(const char * data)
			{
				TINY_ASSERT(data, "data is nullptr")

				return Write((const Byte *)data, strlen(data));
			}

			std::size_t Write(const ByteVector & data)
			{
				return Write(data.data(), data.size());
			}

			std::size_t Write(const std::string & data)
			{
				return Write((const Byte *)data.data(), data.size());
			}

			std::size_t Write(const Byte * data, const std::size_t len)
			{
				TINY_ASSERT(data, "data is nullptr")
				TINY_ASSERT(_compress, "_compress is nullptr")
				TINY_ASSERT(_fileStream, "_fileStream is nullptr")

				if (_mode != TINY_GZIP_FILE_MODE::WRITE)
				{
					TINY_THROW_EXCEPTION(debug::IOError, "write() on read-only GzipFile object");
				}

				if (IsClosed())
				{
					TINY_THROW_EXCEPTION(debug::IOError, "write() on closed GzipFile object");
				}

				if (len > 0)
				{
					_size += len;
					_offset += len;

					_crc = TINY_ZLIB_CRC32(data, len, _crc) & 0xffffffff;

					auto compressData = _compress->Compress<ByteVector>(data, len);

					_fileStream->write((char *)compressData.data(), compressData.size());
				}

				return len;
			}

			ssize_t Seek(ssize_t offset, std::ios_base::seekdir whence = std::ios_base::beg)
			{
				if (whence == std::ios_base::cur)
				{
					offset = _offset + offset;
				}
				else if (whence == std::ios_base::end)
				{
					TINY_THROW_EXCEPTION(debug::ModeError, "Seek from end not supported");
				}

				if (_mode == TINY_GZIP_FILE_MODE::WRITE)
				{
					if (offset < _offset)
					{
						TINY_THROW_EXCEPTION(debug::ModeError, "Negative seek in write mode");
					}

					ssize_t count = offset - _offset;

					ByteVector buf((size_t)count, '\0');

					(void)Write(buf);
				}
				else if (_mode == TINY_GZIP_FILE_MODE::READ)
				{
					if (offset < _offset)
					{
						Rewind();
					}

					ssize_t count = offset - _offset;

					Read<ByteVector>(count);
				}

				return _offset;
			}

			std::string ReadLine(ssize_t size = -1)
			{
				ssize_t readSize;

				if (size < 0)
				{
					ssize_t offset = _offset - _extraStart;

					if (offset < (ssize_t)_extraBuffer.size())
					{
						auto beg  = std::next(_extraBuffer.begin(), offset);
						auto iter = std::find(beg, _extraBuffer.end(), '\n');

						if (iter != _extraBuffer.end())
						{
							std::advance(iter, 1);

							ssize_t count = std::distance(beg, iter);

							_offset += count;
							_extraSize -= count;

							return std::string((const char *)_extraBuffer.data() + offset, (size_t)count);
						}
					}

					size = INT32_MAX;
					readSize = _minReadSize;
				}
				else
				{
					readSize = size;
				}

				std::string  tempStr;

				StringVector tempStrVector;

				while (size != 0)
				{
					tempStr = Read<std::string>(readSize);

					if (tempStr.empty())
					{
						break;
					}

					std::size_t pos = tempStr.find('\n');

					if ((pos != std::string::npos && size < (ssize_t)pos) ||
						(pos == std::string::npos && (ssize_t)tempStr.size() > size))
					{
						tempStrVector.emplace_back(tempStr.data(), (size_t)size);

						UnReadData(tempStr.size() - size);

						break;
					}

					if (pos != std::string::npos)
					{
						tempStrVector.emplace_back(tempStr.data(), pos + 1);

						UnReadData(tempStr.size() - pos - 1);

						break;
					}

					size -= tempStr.size();

					tempStrVector.emplace_back(std::move(tempStr));

					readSize = std::min(size, readSize << 1);
				}

				if (readSize > _minReadSize)
				{
					_minReadSize = std::min(readSize, _minReadSize << 1);
					_minReadSize = std::min(_minReadSize, (ssize_t)512);
				}

				return boost::join(tempStrVector, "");
			}

			template <typename ValueT = std::string>
			ValueT Read(ssize_t size = -1)
			{
				ValueT retVal;

				Read(retVal, size);

				return retVal;
			}

		protected:
			void ReadEOF()
			{
				TINY_ASSERT(_fileStream, "_fileStream is nullptr")

				uint32_t crc = 0;
				uint32_t size = 0;

				_fileStream->clear();
				_fileStream->seekg(-8, std::ios_base::cur);
				_fileStream->read((char *)&crc, 4);

				boost::endian::little_to_native_inplace(crc);

				_fileStream->read((char *)&size, 4);

				boost::endian::little_to_native_inplace(size);

				if (crc != _crc)
				{
					TINY_THROW_EXCEPTION(debug::IOError, TINY_STR_FORMAT("CRC check failed %x != %x", crc, _crc));
				}
				else if (size != (_size & 0xffffffff))
				{
					TINY_THROW_EXCEPTION(debug::IOError, "Incorrect length of data produced");
				}

				char c = '\x00';

				while (_fileStream->good() && c == '\x00')
				{
					c = (char)_fileStream->get();
				}

				if (_fileStream->good())
				{
					_fileStream->seekg(-1, std::ios_base::cur);
				}
			}

			void CheckMode()
			{
				if (_mode == TINY_GZIP_FILE_MODE::READ)
				{
					TINY_THROW_EXCEPTION(debug::ModeError, "ReOpen on read-only GzipFile");
				}
				else if (_mode == TINY_GZIP_FILE_MODE::WRITE)
				{
					TINY_THROW_EXCEPTION(debug::ModeError, "ReOpen on write-only GzipFile");
				}
			}

			void ReadHeader()
			{
				TINY_ASSERT(_fileStream, "_fileStream is nullptr")

				char magicHeader[2] = { 0 };

				_fileStream->read(magicHeader, sizeof(magicHeader));

				if (!_fileStream->good() || magicHeader[0] != '\037' || magicHeader[1] != '\213')
				{
					TINY_THROW_EXCEPTION(debug::IOError, "Not a gzipped file");
				}

				char method;

				_fileStream->get(method);

				if (!_fileStream->good() || method != '\010')
				{
					TINY_THROW_EXCEPTION(debug::IOError, "Unknown compression method");
				}

				char flag;

				_fileStream->get(flag);

				if (!_fileStream->good())
				{
					TINY_THROW_EXCEPTION(debug::IOError, "Not a gzipped file");
				}

				uint32_t time = 0;

				_fileStream->read((char *)&time, 4);

				if (!_fileStream->good())
				{
					TINY_THROW_EXCEPTION(debug::IOError, "Not a gzipped file");
				}

				boost::endian::little_to_native_inplace(time);

				_time = time;
				_fileStream->ignore(2);

				if (_fileStream->fail())
				{
					TINY_THROW_EXCEPTION(debug::IOError, "Not a gzipped file");
				}

				if (flag & TINY_GZIP_FEXTRA)
				{
					int32_t len = _fileStream->get();

					len += 256 * _fileStream->get();

					_fileStream->ignore(len);

					if (_fileStream->fail())
					{
						TINY_THROW_EXCEPTION(debug::IOError, "Not a gzipped file");
					}
				}

				if (flag & TINY_GZIP_FNAME)
				{
					while (true)
					{
						if (_fileStream->get() == '\0' || !_fileStream->good())
						{
							break;
						}

					}

					if (_fileStream->fail())
					{
						TINY_THROW_EXCEPTION(debug::IOError, "Not a gzipped file");
					}
				}

				if (flag & TINY_GZIP_FCOMMENT)
				{
					while (true)
					{
						if (_fileStream->get() == '\0' || !_fileStream->good())
						{
							break;
						}
					}

					if (_fileStream->fail())
					{
						TINY_THROW_EXCEPTION(debug::IOError, "Not a gzipped file");
					}
				}

				if (flag & TINY_GZIP_FHCRC)
				{
					_fileStream->ignore(2);

					if (_fileStream->fail())
					{
						TINY_THROW_EXCEPTION(debug::IOError, "Not a gzipped file");
					}
				}
			}

			void WriteHeader()
			{
				TINY_ASSERT(_fileStream, "_fileStream is nullptr")

				const char magicHeader[] =
				{
					static_cast<const char>('\037'),
					static_cast<const char>('\213')
				};

				_fileStream->write(magicHeader, sizeof(magicHeader));
				_fileStream->put(static_cast<const char>('\010'));

				std::string fileName;

				if (!_fileName.empty())
				{
					system::FileSystem::PathInfo file_name(_fileName.c_str());

					if (file_name.extension().string() == ".gz")
					{
						fileName = file_name.stem().string();
					}
					else
					{
						fileName = file_name.filename().string();
					}
				}

				auto flags = (char)(fileName.empty() ? 0 : TINY_GZIP_FNAME);

				_fileStream->put(flags);

				uint32_t time;

				if (_time)
				{
					time = (uint32_t)_time.get();
				}
				else
				{
					time = (uint32_t)::time(nullptr);
				}

				boost::endian::native_to_little_inplace(time);

				_fileStream->write((const char *)&time, 4);
				_fileStream->put(static_cast<const char>('\002'));
				_fileStream->put(static_cast<const char>('\377'));

				if (!fileName.empty())
				{
					_fileStream->write(fileName.c_str(), fileName.size());
					_fileStream->put(static_cast<const char>('\000'));
				}
			}

			void UnReadData(size_t len)
			{
				_offset -= len;
				_extraSize += len;
			}

			void AddReadData(const ByteVector & data)
			{
				_crc = TINY_ZLIB_CRC32(data, _crc) & 0xffffffff;

				ssize_t offset = _offset - _extraStart;

				if (offset > 0)
				{
					_extraBuffer.erase(_extraBuffer.begin(), std::next(_extraBuffer.begin(), offset));
				}

				_extraBuffer.insert(_extraBuffer.end(), data.begin(), data.end());

				_size += data.size();
				_extraSize += data.size();
				_extraStart = _offset;
			}

			void InternalRead(const size_t len = 1024)
			{
				TINY_ASSERT(_fileStream, "_fileStream is nullptr")

				if (IsClosed() || _fileStream->eof())
				{
					TINY_THROW_EXCEPTION(debug::EOFError, "Reached EOF");
				}

				if (_isNewMember)
				{
					auto pos = _fileStream->tellg();

					_fileStream->seekg(0, std::ios_base::end);

					if (pos == _fileStream->tellg())
					{
						TINY_THROW_EXCEPTION(debug::EOFError, "Reached EOF");
					}
					else
					{
						_fileStream->seekg(pos, std::ios_base::beg);
					}

					_size = 0;
					_crc = TINY_ZLIB_CRC32("") & 0xffffffff;

					ReadHeader();

					_decompress = std::make_unique<ZLibDecompress>(-ZLib::ZLIB_MAX_WBITS);

					_isNewMember = false;
				}

				TINY_THROW_EXCEPTION_IF(_decompress == nullptr, debug::Nullptr, "_decompress is nullptr")

				ByteVector buffer;
				ByteVector uncompress;

				buffer.resize(len);

				_fileStream->read((char *)buffer.data(), len);

				buffer.resize(static_cast<size_t>(_fileStream->gcount()));

				if (buffer.empty())
				{
					uncompress = _decompress->Flush<ByteVector>();

					ReadEOF();

					AddReadData(uncompress);

					TINY_THROW_EXCEPTION(debug::EOFError, "Reached EOF");
				}

				uncompress = _decompress->Decompress<ByteVector>(buffer);

				AddReadData(uncompress);

				const ByteVector & unusedData = _decompress->GetUnusedData();

				if (!unusedData.empty())
				{
					_fileStream->seekg(8 - (ssize_t)unusedData.size(), std::ios_base::cur);

					ReadEOF();

					_isNewMember = true;
				}
			}

			template <typename ValueT>
			void Read(ValueT & retVal, ssize_t size)
			{
				if (IsClosed())
				{
					TINY_THROW_EXCEPTION(debug::ValueError, "I/O operation on closed file");
				}

				if (_mode != TINY_GZIP_FILE_MODE::READ)
				{
					TINY_THROW_EXCEPTION(debug::IOError, "read() on write-only GzipFile object");
				}

				if (_extraSize <= 0 && !_fileStream)
				{
					return;
				}

				std::size_t readSize = 1024;

				if (size < 0)
				{
					try
					{
						while (true)
						{
							InternalRead(readSize);

							readSize = std::min(_maxReadChunk, readSize << 1);
						}
					}
					catch (debug::EOFError & e)
					{
						size = _extraSize;
					}
				}
				else
				{
					try
					{
						while (size > _extraSize)
						{
							InternalRead(readSize);

							readSize = std::min(_maxReadChunk, readSize << 1);
						}
					}
					catch (debug::EOFError & e)
					{
						if (size > _extraSize)
						{
							size = _extraSize;
						}
					}
				}

				ssize_t offset = _offset - _extraStart;

				retVal.assign(std::next(_extraBuffer.begin(), offset), std::next(_extraBuffer.begin(), offset + size));

				_offset += size;
				_extraSize -= size;
			}

		public:
			static const std::size_t _maxReadChunk = 10 * TINY_MB;

		protected:
			bool _isNewMember{ false };

			uLong   _crc{ 0 };
			size_t  _size{ 0 };
			ssize_t _offset{ 0 };
			ssize_t _extraSize{ 0 };
			ssize_t _extraStart{ 0 };
			ssize_t _minReadSize{ 0 };

			TINY_GZIP_FILE_MODE _mode{ TINY_GZIP_FILE_MODE::NONE };

			ByteVector _extraBuffer{ };

			std::string _fileName{ };

			boost::optional<time_t> _time{ };

			std::shared_ptr<std::iostream> _fileStream{ };
			std::unique_ptr<ZLibCompress> _compress{ };
			std::unique_ptr<ZLibDecompress> _decompress{ };
		};
	}
}



#endif // __TINY_CORE__COMPRESS__GZIP__H__
