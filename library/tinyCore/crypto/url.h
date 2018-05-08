#ifndef __TINY_CORE__CRYPTO__URL__H__
#define __TINY_CORE__CRYPTO__URL__H__


/**
 *
 *  作者: hm
 *
 *  说明: url编码
 *
 */


#include <tinyCore/utilities/digital.h>


namespace tinyCore
{
	namespace crypto
	{
		class URL
		{
		public:
			static std::string Encode(const char * value)
			{
				TINY_ASSERT(value, "value is nullptr")

				return Encode((Byte *)value, strlen(value));
			}

			static std::string Encode(const std::string & value)
			{
				return Encode((Byte *)value.c_str(), value.size());
			}

			static std::string Encode(const Byte * value, std::size_t size)
			{
				TINY_ASSERT(value, "value is nullptr")

				Byte byte;

				std::string tempStr;

				for (size_t i = 0; i < size; ++i)
				{
					byte = value[i];

					if (isalnum(byte))
					{
						tempStr += byte;
					}
					else if (byte == ' ')
					{
						tempStr += "+";
					}
					else if (byte == '-' || byte == '_' || byte == '.'  || byte == '!' ||
							 byte == '~' || byte == '*' || byte == '\'' || byte == '(' || byte == ')')
					{
						tempStr += byte;
					}
					else
					{
						tempStr += "%";
						tempStr += TINY_DIGITAL_TO_HEX(static_cast<Byte>(byte >> 4));
						tempStr += TINY_DIGITAL_TO_HEX(static_cast<Byte>(byte % 16));
					}
				}

				return tempStr;
			}

			static std::string Decode(const char * value)
			{
				TINY_ASSERT(value, "value is nullptr")

				return Decode((Byte *)value, strlen(value));
			}

			static std::string Decode(const std::string & value)
			{
				return Decode((Byte *)value.c_str(), value.size());
			}

			static std::string Decode(const Byte * value, std::size_t size)
			{
				TINY_ASSERT(value, "value is nullptr")

				std::string tempStr;

				for (size_t i = 0; i < size; ++i)
				{
					if (value[i] == '+')
					{
						tempStr += ' ';
					}
					else if (value[i] == '%')
					{
						Byte high = TINY_DIGITAL_TO_BYTE(value[++i]);
						Byte low  = TINY_DIGITAL_TO_BYTE(value[++i]);

						tempStr += high * 16 + low;
					}
					else
					{
						tempStr += value[i];
					}
				}

				return tempStr;
			}
		};
	}
}


#define TINY_URL_ENCODE(value, ...) tinyCore::crypto::URL::Encode(value, ##__VA_ARGS__)
#define TINY_URL_DECODE(value, ...) tinyCore::crypto::URL::Decode(value, ##__VA_ARGS__)


#endif // __TINY_CORE__CRYPTO__URL__H__
