#ifndef __TINY_CORE__CONTAINER__QUEUE__H__
#define __TINY_CORE__CONTAINER__QUEUE__H__


/**
 *
 *  作者: hm
 *
 *  说明: 队列
 *
 */


#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace container
	{
		template <typename TypeT, const size_t SIZE = 4096>
		class CircularQueue
		{
		public:
			CircularQueue()
			{
				memset(_fill,  0, sizeof(_fill));
				memset(_queue, 0, sizeof(_queue));
			}

			inline TypeT * ReadLock()
			{
				if (_fill[_readIndex])
				{
					return &_queue[_readIndex];
				}

				return nullptr;
			}

			inline int64_t ReadUnlock()
			{
				if (_fill[_readIndex])
				{
					memset(&_queue[_readIndex], 0, sizeof(TypeT));

					_fill[_readIndex] = 0;

					_readIndex = (_readIndex + 1) % SIZE;

					return _readIndex;
				}

				return -1;
			}

			inline TypeT * WriteLock()
			{
				if (_fill[_writeIndex])
				{
					return nullptr;
				}
				else
				{
					return &_queue[_writeIndex];
				}
			}

			inline int64_t WriteUnlock()
			{
				if (_fill[_writeIndex])
				{
					return -1;
				}

				_fill[_writeIndex] = true;

				_writeIndex = (_writeIndex + 1) % SIZE;

				return _writeIndex;
			}

		protected:
			bool  _fill[SIZE]{ false };

			TypeT _queue[SIZE]{ };

			int64_t _readIndex{ 0 };
			int64_t _writeIndex{ 0 };
		};

		template<typename TypeT>
		class LockQueue
		{
			typedef struct DATA
			{
				bool fill{ false };

				TypeT data;
			}DATA;

		public:
			explicit LockQueue(std::size_t size = TINY_KB) : _size(size), _mask(size - 1)
			{
				if ((size < 2) || ((size & (size - 1)) != 0))
				{
					TINY_THROW_EXCEPTION(debug::SizeError, "Size Must Be Power Of Two")
				}

				_data = new DATA[size];
			}

			~LockQueue()
			{
				delete[] _data;
			}

			bool Write(const TypeT & data)
			{
				std::lock_guard<std::mutex> lock(_lock);

				DATA * dataPtr = &_data[_writePos];

				if (dataPtr->fill)
				{
					return false;
				}

				_writePos = (_writePos + 1) % _size;

				dataPtr->fill = true;
				dataPtr->data = data;

				return true;
			}

			bool Read(TypeT & data)
			{
				std::lock_guard<std::mutex> lock(_lock);

				DATA * dataPtr = &_data[_readPos];

				if (!dataPtr->fill)
				{
					return false;
				}

				_readPos = (_readPos + 1) % _size;

				dataPtr->fill = false;

				data = dataPtr->data;

				return true;
			}

			bool WriteMove(TypeT && data)
			{
				std::lock_guard<std::mutex> lock(_lock);

				DATA * dataPtr = &_data[_writePos];

				if (dataPtr->fill)
				{
					return false;
				}

				_writePos = (_writePos + 1) % _size;

				dataPtr->fill = true;
				dataPtr->data = std::move(data);

				return true;
			}

			bool ReadMove(TypeT & data)
			{
				std::lock_guard<std::mutex> lock(_lock);

				DATA * dataPtr = &_data[_readPos];

				if (!dataPtr->fill)
				{
					return false;
				}

				_readPos = (_readPos + 1) % _size;

				dataPtr->fill = false;

				data = std::move(dataPtr->data);

				return true;
			}

			bool Empty()
			{
				return _readPos == _writePos;
			}

		protected:
			DATA * _data{ nullptr };

			std::mutex _lock{ };

			std::size_t _size{ 0 };
			std::size_t _mask{ 0 };
			std::size_t _readPos{ 0 };
			std::size_t _writePos{ 0 };
		};

		template<typename TypeT>
		class BoundedQueue
		{
			typedef struct DATA
			{
				TypeT data;

				std::atomic<size_t> sequence;
			}DATA;

		public:
			explicit BoundedQueue(std::size_t size = TINY_KB) : _size(size), _mask(size - 1), _readPos(0), _writePos(0)
			{
				if ((_size < 2) || ((_size & (_size - 1)) != 0))
				{
					TINY_THROW_EXCEPTION(debug::SizeError, "Size Must Be Power Of Two")
				}

				_data = new DATA[_size];

				for (std::size_t i = 0; i != _size; i += 1)
				{
					_data[i].sequence.store(i, std::memory_order_relaxed);
				}

				_readPos.store(0, std::memory_order_relaxed);
				_writePos.store(0, std::memory_order_relaxed);
			}

			~BoundedQueue()
			{
				delete[] _data;
			}

			bool Write(const TypeT & data)
			{
				DATA * dataPtr = nullptr;

				std::size_t pos = _writePos.load(std::memory_order_relaxed);

				while (true)
				{
					dataPtr = &_data[pos & _mask];

					std::size_t sequence = dataPtr->sequence.load(std::memory_order_acquire);

					intptr_t different = static_cast<intptr_t>(sequence) - static_cast<intptr_t>(pos);

					if (different == 0)
					{
						if (_writePos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
						{
							break;
						}
					}
					else if (different < 0)
					{
						return false;
					}
					else
					{
						pos = _writePos.load(std::memory_order_relaxed);
					}
				}

				dataPtr->data = data;
				dataPtr->sequence.store(pos + 1, std::memory_order_release);

				return true;
			}

			bool Read(TypeT & data)
			{
				DATA * dataPtr = nullptr;

				std::size_t pos = _readPos.load(std::memory_order_relaxed);

				while (true)
				{
					dataPtr = &_data[pos & _mask];

					std::size_t sequence  = dataPtr->sequence.load(std::memory_order_acquire);

					intptr_t different = static_cast<intptr_t>(sequence) - static_cast<intptr_t>(pos + 1);

					if (different == 0)
					{
						if (_readPos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
						{
							break;
						}
					}
					else if (different < 0)
					{
						return false;
					}
					else
					{
						TINY_SLEEP_MS(100)

						pos = _readPos.load(std::memory_order_relaxed);
					}
				}

				data = dataPtr->data;
				dataPtr->sequence.store(pos + _mask + 1, std::memory_order_release);

				return true;
			}

			bool WriteMove(TypeT && data)
			{
				DATA * dataPtr = nullptr;

				std::size_t pos = _writePos.load(std::memory_order_relaxed);

				while (true)
				{
					dataPtr = &_data[pos & _mask];

					std::size_t sequence  = dataPtr->sequence.load(std::memory_order_acquire);

					intptr_t different = static_cast<intptr_t>(sequence) - static_cast<intptr_t>(pos);

					if (different == 0)
					{
						if (_writePos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
						{
							break;
						}
					}
					else if (different < 0)
					{
						return false;
					}
					else
					{
						pos = _writePos.load(std::memory_order_relaxed);
					}
				}

				dataPtr->data = std::move(data);
				dataPtr->sequence.store(pos + 1, std::memory_order_release);

				return true;
			}

			bool ReadMove(TypeT & data)
			{
				DATA * dataPtr = nullptr;

				std::size_t pos = _readPos.load(std::memory_order_relaxed);

				while (true)
				{
					dataPtr = &_data[pos & _mask];

					std::size_t sequence  = dataPtr->sequence.load(std::memory_order_acquire);

					intptr_t different = static_cast<intptr_t>(sequence) - static_cast<intptr_t>(pos + 1);

					if (different == 0)
					{
						if (_readPos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
						{
							break;
						}
					}
					else if (different < 0)
					{
						return false;
					}
					else
					{
						TINY_SLEEP_MS(100)

						pos = _readPos.load(std::memory_order_relaxed);
					}
				}

				data = std::move(dataPtr->data);
				dataPtr->sequence.store(pos + _mask + 1, std::memory_order_release);

				return true;
			}

			bool Empty()
			{
				std::size_t readPos;
				std::size_t tempPos;
				std::size_t writePos;

				do
				{
					readPos  = _readPos.load(std::memory_order_acquire);
					writePos = _writePos.load(std::memory_order_acquire);
					tempPos  = _writePos.load(std::memory_order_relaxed);
				}while (writePos != tempPos);

				return readPos == writePos;
			}

		protected:
			DATA * _data{ nullptr };

			std::size_t _size{ 0 };
			std::size_t _mask{ 0 };

			std::atomic<std::size_t> _readPos;
			std::atomic<std::size_t> _writePos;
		};
	}
}


#endif // __TINY_CORE__CONTAINER__QUEUE__H__
