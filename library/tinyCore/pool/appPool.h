#ifndef __TINY_CORE__POOL__APP_POOL__H__
#define __TINY_CORE__POOL__APP_POOL__H__


/**
 *
 *  作者: hm
 *
 *  说明: 应用池
 *
 */


#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace pool
	{
		template <typename ValueT, uint32_t CHUNK_SIZE = 32>
		class AppPool
		{
			struct ChunkList;

			struct Chunk
			{
				char buffer[sizeof(ValueT)];

				bool isUsed;

				size_t size;
				size_t index;

				Chunk * prev;
				Chunk * next;

				ChunkList * parent;
			};

			struct ChunkList
			{
				size_t used;

				ChunkList * prev;
				ChunkList * next;

				Chunk chunks[CHUNK_SIZE];
			};

		public:
			AppPool() = default;

			~AppPool()
			{
				while (_listHead)
				{
					DelChunkList(_listHead);
				}
			}

			ValueT * Create()
			{
				Chunk * chunk = CreateChunk();

				TINY_THROW_EXCEPTION_IF(chunk == nullptr, debug::Nullptr, "create chunk error")

				if (chunk)
				{
					return new(chunk->buffer) ValueT();
				}
				else
				{
					return nullptr;
				}
			}

			template <typename... Args>
			ValueT * Create(Args... args)
			{
				Chunk * chunk = CreateChunk();

				TINY_THROW_EXCEPTION_IF(chunk == nullptr, debug::Nullptr, "create chunk error")

				if (chunk)
				{
					return new(chunk->buffer) ValueT(args...);
				}
				else
				{
					return nullptr;
				}
			}

			void Recover(ValueT * t)
			{
				TINY_ASSERT(t, "t is nullptr")

				if (t)
				{
					t->~ValueT();

					RecoverChunk((Chunk *)t);
				}
			}

			size_t Size()
			{
				return _count == 0 ? CHUNK_SIZE : _count * CHUNK_SIZE;
			}

		protected:
			Chunk * CreateChunk()
			{
				if (_chunkHead == nullptr)
				{
					AddChunkList(new ChunkList);
				}

				auto ret = _chunkHead;

				DelChunk(_chunkHead);

				if (ret->isUsed)
				{
					TINY_THROW_EXCEPTION(debug::ValueError, "Chunk Already Used")

					return nullptr;
				}

				ret->isUsed = true;

				++ret->parent->used;

				return ret;
			}

			void RecoverChunk(Chunk * chunk)
			{
				TINY_ASSERT(chunk, "chunk is nullptr")

				if (chunk)
				{
					TINY_THROW_EXCEPTION_IF(!chunk->isUsed, debug::ValueError, "Chunk Already Recover")

					chunk->isUsed = false;

					--chunk->parent->used;

					if (chunk->parent->used == 0 && _count > 1)
					{
						DelChunkList(chunk->parent);
					}
					else
					{
						AddChunk(chunk);
					}
				}
			}

			void AddChunkList(ChunkList * list)
			{
				TINY_ASSERT(list, "list is nullptr")

				if (list)
				{
					for (size_t i = 0; i < CHUNK_SIZE; ++i)
					{
						list->chunks[i].size   = sizeof(Chunk);
						list->chunks[i].prev   = nullptr;
						list->chunks[i].next   = nullptr;
						list->chunks[i].index  = i;
						list->chunks[i].isUsed = false;
						list->chunks[i].parent = list;

						AddChunk(&(list->chunks[i]));
					}

					list->used = 0;
					list->prev = nullptr;
					list->next = _listHead;

					if (_listHead)
					{
						_listHead->prev = list;
					}

					_listHead = list;

					++_count;
				}
			}

			void DelChunkList(ChunkList * list)
			{
				TINY_ASSERT(list, "list is nullptr")

				if (list)
				{
					for (size_t i = 0; i < CHUNK_SIZE; ++i)
					{
						DelChunk(&(list->chunks[i]));
					}

					if (list->next)
					{
						list->next->prev = list->prev;
					}

					if (list->prev)
					{
						list->prev->next = list->next;
					}

					if (list == _listHead)
					{
						_listHead = list->next;
					}

					delete list;

					--_count;
				}
			}

			void AddChunk(Chunk * chunk)
			{
				TINY_ASSERT(chunk, "chunk is nullptr")

				if (chunk)
				{
					chunk->next = _chunkHead;

					if (_chunkHead)
					{
						_chunkHead->prev = chunk;
					}

					_chunkHead = chunk;
				}
			}

			void DelChunk(Chunk * chunk)
			{
				TINY_ASSERT(chunk, "chunk is nullptr")

				if (chunk)
				{
					if (chunk->next)
					{
						chunk->next->prev = chunk->prev;
					}

					if (chunk->prev)
					{
						chunk->prev->next = chunk->next;
					}

					if (chunk == _chunkHead)
					{
						_chunkHead = chunk->next;
					}

					chunk->next = nullptr;
					chunk->prev = nullptr;
				}
			}

		protected:
			int32_t _count{ 0 };

			Chunk * _chunkHead{ nullptr };

			ChunkList * _listHead{ nullptr };
		};
	}
}


#endif // __TINY_CORE__POOL__APP_POOL__H__
