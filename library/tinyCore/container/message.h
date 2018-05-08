#ifndef __TINY_CORE__CONTAINER__MESSAGE__H__
#define __TINY_CORE__CONTAINER__MESSAGE__H__


/**
 *
 *  作者: hm
 *
 *  说明: 数据缓冲区
 *
 */


#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace container
	{
		class Message
		{
		public:
			Message()
			{
				_storage.resize(4 * TINY_KB);
			}

			explicit Message(const std::size_t initialSize)
			{
				_storage.resize(initialSize);
			}

			explicit Message(const char * data) : _storage((const Byte *)data, (const Byte *)data + strlen(data))
			{

			}

			explicit Message(const std::string & data) : _storage((const Byte *)data.data(), (const Byte *)data.data() + data.size())
			{

			}

			Message(Message && rhs) noexcept : _readPos(rhs._readPos), _writePos(rhs._writePos), _storage(rhs.Move())
			{

			}

			void Reset()
			{
				_readPos  = 0;
				_writePos = 0;

				memset(BasePointer(), 0, _storage.size());
			}

			void Normalize()
			{
				if (_readPos > 0)
				{
					if (_readPos != _writePos)
					{
						memmove(BasePointer(), ReadPointer(), UnReadSize());
					}

					_writePos = UnReadSize();
					_readPos  = 0;
				}
			}

			void Resize(size_t bytes)
			{
				_storage.resize(bytes);
			}

			void Write(const char * data)
			{
				TINY_ASSERT(data, "data is nullptr");

				Write((const Byte*)data, strlen(data));
			}

			void Write(const std::string & data)
			{
				Write((const Byte*)data.data(), data.size());
			}

			void Write(const Byte * data, size_t size)
			{
				TINY_ASSERT(data, "data is nullptr");

				if (size > 0)
				{
					memcpy(WritePointer(), data, size);

					WriteCompleted(size);
				}
			}

			void ReadCompleted(size_t bytes)
			{
				_readPos += bytes;

				TINY_ASSERT(_readPos <= _writePos, "wtf");
			}

			void WriteCompleted(size_t bytes)
			{
				_writePos += bytes;

				TINY_ASSERT(_writePos <= _storage.size(), "wtf");
			}

			// 确保可用空间
			void EnsureFreeSpace()
			{
				if (UnWriteSize() == 0)
				{
					_storage.resize(_storage.size() * 3 / 2);
				}
			}

			void EnsureFreeSpace(size_t bytes)
			{
				if (UnWriteSize() < bytes)
				{
					_storage.resize(Size() + bytes - UnWriteSize());
				}
			}

			size_t Size() const
			{
				return _writePos;
			}

			size_t MaxSize() const
			{
				return _storage.size();
			}

			size_t UnReadSize() const
			{
				return _writePos - _readPos;
			}

			size_t UnWriteSize() const
			{
				return _storage.size() - _writePos;
			}

			Byte * BasePointer()
			{
				return _storage.data();
			}

			Byte * ReadPointer()
			{
				return BasePointer() + _readPos;
			}

			Byte * WritePointer()
			{
				return BasePointer() + _writePos;
			}

			const char * Data()
			{
				return (const char *)_storage.data();
			}

			ByteVector && Move()
			{
				_readPos  = 0;
				_writePos = 0;

				return std::move(_storage);
			}

			Message & operator=(const char * data)
			{
				Reset();

				Write(data);

				return *this;
			}

			Message & operator=(const std::string & data)
			{
				Reset();

				Write(data);

				return *this;
			}

			Message & operator=(Message && rhs) noexcept
			{
				_readPos  = rhs._readPos;
				_writePos = rhs._writePos;
				_storage  = rhs.Move();

				return *this;
			}

			Message & operator=(const Message & rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}

				_readPos  = rhs._readPos;
				_writePos = rhs._writePos;
				_storage  = rhs._storage;

				return *this;
			}

		protected:
			size_t _readPos{ 0 };
			size_t _writePos{ 0 };

			ByteVector _storage{ };
		};
	}
}


#endif // __TINY_CORE__CONTAINER__MESSAGE__H__
