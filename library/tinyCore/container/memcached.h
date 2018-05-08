#ifndef __TINY_CORE__CONTAINER__MEMCACHED__H__
#define __TINY_CORE__CONTAINER__MEMCACHED__H__


/**
 *
 *  作者: hm
 *
 *  说明: memcached
 *
 */


#include <libmemcached/memcached.h>

#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace container
	{
		class Memcached
		{
		public:
			bool Initialize(const char * host = "127.0.0.1", uint16_t port = 11211)
			{
				TINY_ASSERT(_memcached == nullptr, "memcached already initialize")

				_memcached = memcached_create(nullptr);

				if (_memcached == nullptr)
				{
					_errorMsg = memcached_error(_memcached);

					return false;
				}

				memcached_return_t ret;

				memcached_server_st * server = memcached_server_list_append(nullptr, host, port, &ret);

				if (server == nullptr)
				{
					_errorMsg = memcached_strerror(_memcached, ret);

					return false;
				}

				memcached_server_push(_memcached, server);
				memcached_server_list_free(server);

				/* 使用NO-BLOCK, 防止memcached倒掉时挂死 */
				ret = memcached_behavior_set(_memcached, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);

				if (ret != MEMCACHED_SUCCESS)
				{
					_errorMsg = memcached_strerror(_memcached, ret);

					return false;
				}

				/* 使用二进制协议传输 */
				ret = memcached_behavior_set(_memcached, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1);

				if (ret != MEMCACHED_SUCCESS)
				{
					_errorMsg = memcached_strerror(_memcached, ret);

					return false;
				}

				ret = memcached_behavior_set(_memcached, MEMCACHED_BEHAVIOR_DISTRIBUTION,
														 MEMCACHED_DISTRIBUTION_CONSISTENT);

				if (ret != MEMCACHED_SUCCESS)
				{
					_errorMsg = memcached_strerror(_memcached, ret);

					return false;
				}

				ret = memcached_behavior_set(_memcached, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 20);

				if (ret != MEMCACHED_SUCCESS)
				{
					_errorMsg = memcached_strerror(_memcached, ret);

					return false;
				}

				ret = memcached_behavior_set(_memcached, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, 5);

				if (ret != MEMCACHED_SUCCESS)
				{
					_errorMsg = memcached_strerror(_memcached, ret);

					return false;
				}

				ret = memcached_behavior_set(_memcached, MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS, 1);

				if (ret != MEMCACHED_SUCCESS)
				{
					_errorMsg = memcached_strerror(_memcached, ret);

					return false;
				}

				return true;
			}

			bool GetValue(const char * key, std::string & value)
			{
				TINY_ASSERT(key, "key is nullptr");
				TINY_ASSERT(_memcached, "memcached not initialize or nullptr");

				uint32_t flag;
				std::size_t len;
				memcached_return_t ret;

				char * tempStr = memcached_get(_memcached, key, strlen(key), &len, &flag, &ret);

				if (ret == MEMCACHED_SUCCESS)
				{
					value = tempStr;
				}
				else
				{
					_errorMsg = memcached_strerror(_memcached, ret);
				}

				if (tempStr)
				{
					free(tempStr);
				}

				return (ret == MEMCACHED_SUCCESS);
			}

			bool SetValue(const char * key, const std::string & value)
			{
				TINY_ASSERT(key, "key is nullptr");
				TINY_ASSERT(_memcached, "memcached not initialize or nullptr");

				uint32_t flags = 0;

				auto ret = memcached_set(_memcached, key, strlen(key), value.c_str(), value.size(), _expiration, flags);

				if (ret != MEMCACHED_SUCCESS)
				{
					_errorMsg = memcached_strerror(_memcached, ret);
				}

				return (ret == MEMCACHED_SUCCESS);
			}

			void SetExpiration(const std::time_t expiration)
			{
				_expiration = expiration;
			}

			bool DeleteValue(const std::string & key)
			{
				TINY_ASSERT(_memcached, "memcached not initialize or nullptr");

				auto ret = memcached_delete(_memcached, key.c_str(), key.size(), _expiration);

				if (ret != MEMCACHED_SUCCESS)
				{
					_errorMsg = memcached_strerror(_memcached, ret);
				}

				return (ret == MEMCACHED_SUCCESS);
			}

			void Release()
			{
				if (_memcached)
				{
					memcached_free(_memcached);

					_memcached = nullptr;
				}
			}

			const std::string & LastError() const
			{
				return _errorMsg;
			}

		protected:
			std::time_t _expiration{ 0 };

			std::string _errorMsg{ };

			memcached_st * _memcached{ nullptr };
		};
	}
}


#endif // __TINY_CORE__CONTAINER__MEMCACHED__H__
