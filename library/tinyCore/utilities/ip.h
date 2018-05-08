#ifndef __TINY_CORE__UTILITIES__IP__H__
#define __TINY_CORE__UTILITIES__IP__H__


/**
 *
 *  作者: hm
 *
 *  说明: ip转换
 *
 */


#include <boost/asio.hpp>

#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace utilities
	{
		// ntohl(uint32_t __netlong)  // 网络字节序 --> 主机字节序
		// htonl(uint32_t __hostlong) // 主机字节序 --> 网络字节序

		// ntohs(uint16_t __netshort)  // 网络字节序 --> 主机字节序
		// htons(uint16_t __hostshort) // 主机字节序 --> 网络字节序

		// in_addr_t inet_addr(const char * __cp);  // 返回网络字节序
		// in_addr_t inet_network(const char * __cp);  // 返回主机字节序

		// char * inet_ntoa(struct in_addr __in);  // 网络字节序 --> IP字符串
		// char * inet_ntop(int af, const void * __restrict cp, char * __restrict buf, socklen_t len);  // 网络字节序 --> IP字符串

		// int inet_aton(const char * __cp, struct in_addr * __inp);  // 返回网络字节序
		// int inet_pton(int af, const char * __restrict cp, void * __restrict buf); // IP字符串 --> 网络字节序

		class TINY_API HostByte
		{
		public:
			static bool FromString(const std::string & value, uint32_t * head, uint32_t * tail)
			{
				if (head == nullptr || tail == nullptr)
				{
					return false;
				}

				std::size_t pos = value.find('-');

				if (pos != std::string::npos)
				{
					*head = FromString(value.substr(0, pos).data());
					*tail = FromString(value.substr(pos + 1).data());

					if (*head > *tail)
					{
						assert(false);

						return false;
					}
				}

				pos = value.find('-');

				if (pos == std::string::npos)
				{
					*head = FromString(value.data());

					*tail = *head;
				}
				else
				{
					if (value.find('.', pos + 1) == std::string::npos)
					{
						uint32_t mask = (uint32_t)strtol(value.substr(pos + 1).c_str(), nullptr, 10);

						if (mask > 32)
						{
							assert(false);

							return false;
						}

						*head = FromString(value.substr(0, pos).data());

						*tail = *head | (~htonl(0xFFFFFFFF << (32 - mask)));
					}
					else
					{
						*head = FromString(value.substr(0, pos).data());
						*tail = FromString(value.substr(pos + 1).data());

						if (*head > *tail)
						{
							assert(false);

							return false;
						}
					}
				}

				return true;
			}

			static uint32_t FromString(const char *  value)
			{
				return inet_network(value);  // 返回主机字节序
			}

			static uint32_t ToNetworkByte(const uint32_t value)
			{
				return htonl(value);  // 主机字节序 --> 网络字节序
			}

			static uint32_t ToNetworkByte(const char * value)
			{
				return inet_addr(value);  // 返回网络字节序
			}

			static std::string ToString(uint32_t value)
			{
				std::string tempStr;

				tempStr += std::to_string(value >> 24 & 0xFF);
				tempStr += ".";
				tempStr += std::to_string((value >> 16) & 0xFF);
				tempStr += ".";
				tempStr += std::to_string((value >>  8) & 0xFF);
				tempStr += ".";
				tempStr += std::to_string((value >>  0) & 0xFF);

				return tempStr;
			}
		};

		class TINY_API NetworkByte
		{
		public:
			static bool FromString(const std::string & value, uint32_t * head, uint32_t * tail)
			{
				if (head == nullptr || tail == nullptr)
				{
					return false;
				}

				std::size_t pos = value.find('-');

				if (pos != std::string::npos)
				{
					*head = FromString(value.substr(0, pos).data());
					*tail = FromString(value.substr(pos + 1).data());

					if (*head > *tail)
					{
						assert(false);

						return false;
					}
				}

				pos = value.find('-');

				if (pos == std::string::npos)
				{
					*head = FromString(value.data());

					*tail = *head;
				}
				else
				{
					if (value.find('.', pos + 1) == std::string::npos)
					{
						uint32_t mask = (uint32_t)strtol(value.substr(pos + 1).c_str(), nullptr, 10);

						if (mask > 32)
						{
							assert(false);

							return false;
						}

						*head = FromString(value.substr(0, pos).data());

						*tail = *head | (~htonl(0xFFFFFFFF << (32 - mask)));
					}
					else
					{
						*head = FromString(value.substr(0, pos).data());
						*tail = FromString(value.substr(pos + 1).data());

						if (*head > *tail)
						{
							assert(false);

							return false;
						}
					}
				}

				return true;
			}

			static uint32_t FromString(const char *  value)
			{
				return inet_addr(value);  // 返回网络字节序
			}

			static uint32_t ToHostByte(const uint32_t value)
			{
				return ntohl(value);  // 网络字节序 --> 主机字节序
			}

			static uint32_t ToHostByte(const char * value)
			{
				return inet_network(value);  // 返回主机字节序
			}

			static std::string ToString(const uint32_t value)
			{
				return HostByte::ToString(ntohl(value));
			}
		};

		class TINY_API IP
		{
		public:
			template<typename TypeT>
			static bool IsValid(const TypeT & value)
			{
				boost::system::error_code ec;

				boost::asio::ip::address::from_string(value, ec);

				return !ec;
			}

			static bool HostByName(const char * name, StringSet & res)
			{
				if (name == nullptr)
				{
					return false;
				}

				hostent * hostList = gethostbyname(name);

				if (hostList)
				{
					for (int i = 0; hostList->h_addr_list[i]; ++i)
					{
						uint32_t host = 0;

						memcpy(&host, hostList->h_addr_list[i], (std::size_t)hostList->h_length);

						res.insert(NetworkByte::ToString(host));
					}

					return true;
				}

				return false;
			}

			static bool HostByName(const char * name, StringVector & res)
			{
				if (name == nullptr)
				{
					return false;
				}

				hostent * hostList = gethostbyname(name);

				if (hostList)
				{
					for (int i = 0; hostList->h_addr_list[i]; ++i)
					{
						uint32_t host = 0;

						memcpy(&host, hostList->h_addr_list[i], (std::size_t)hostList->h_length);

						res.push_back(NetworkByte::ToString(host));
					}

					return true;
				}

				return false;
			}
		};
	}
}


#define TINY_IP_IS_VALID(value) tinyCore::utilities::IP::IsValid(value)
#define TINY_IP_HOST_BY_NAME(value, res) tinyCore::utilities::IP::HostByName(value, res)

#define TINY_HOST_BYTE_TO_STRING(value) tinyCore::utilities::HostByte::ToString(value)
#define TINY_HOST_BYTE_TO_NETWORK(value) tinyCore::utilities::HostByte::ToNetworkByte(value)
#define TINY_HOST_BYTE_FROM_STRING(value, ...) tinyCore::utilities::HostByte::FromString(value, ##__VA_ARGS__)

#define TINY_NETWORK_BYTE_TO_HOST(value) tinyCore::utilities::NetworkByte::ToHostByte(value)
#define TINY_NETWORK_BYTE_TO_STRING(value) tinyCore::utilities::NetworkByte::ToString(value)
#define TINY_NETWORK_BYTE_FROM_STRING(value, ...) tinyCore::utilities::NetworkByte::FromString(value, ##__VA_ARGS__)


#endif // __TINY_CORE__UTILITIES__IP__H__
