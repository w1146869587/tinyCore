#ifndef __TINY_CORE__UTILITIES__DIGITAL__H__
#define __TINY_CORE__UTILITIES__DIGITAL__H__


/**
 *
 *  作者: hm
 *
 *  说明: 数字处理
 *
 */


#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace utilities
	{
		class TINY_API Digital
		{
		public:
			static Byte ToHex(const Byte byte)
			{
				return static_cast<Byte>(byte > 9 ? byte + 55 : byte + 48);
			}

			static Byte ToByte(const Byte hex)
			{
				if (hex >= 'A' && hex <= 'Z')
				{
					return static_cast<Byte>(hex - 'A' + 10);
				}
				else if (hex >= 'a' && hex <= 'z')
				{
					return static_cast<Byte>(hex - 'a' + 10);
				}
				else if (hex >= '0' && hex <= '9')
				{
					return static_cast<Byte>(hex - '0');
				}
				else
				{
					return hex;
				}
			}

			template <typename TypeT>
			static std::string ToHexString(const TypeT value, const char * fmt = "%02x")
			{
				char result[32] = { 0 };

				return ToHexString(value, result, fmt);
			}

			template <typename TypeT>
			static char * ToHexString(const TypeT value, char * buff, const char * fmt = "%02x")
			{
				if (buff && fmt)
				{
					snprintf(buff, strlen(buff), fmt, value);
				}

				return buff;
			}
		};
	}
}


#define TINY_DIGITAL_TO_HEX(value) tinyCore::utilities::Digital::ToHex(value)
#define TINY_DIGITAL_TO_BYTE(value) tinyCore::utilities::Digital::ToByte(value)

#define TINY_DIGITAL_TO_HEX_STR(value, ...) tinyCore::utilities::Digital::ToHexString(value, ##__VA_ARGS__)



#endif // __TINY_CORE__UTILITIES__DIGITAL__H__
