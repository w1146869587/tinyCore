#ifndef __TINY_CORE__CRYPTO__BASE64__H__
#define __TINY_CORE__CRYPTO__BASE64__H__


/**
 *
 *  作者: hm
 *
 *  说明: base64编码
 *
 */


#include <tinyCore/utilities/string.h>


namespace tinyCore
{
	namespace crypto
	{
		static char Tiny_Base64_Encode_Table[64] =
		{
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
			'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
			'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
			'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
			'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
			'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
			'w', 'x', 'y', 'z', '0', '1', '2', '3',
			'4', '5', '6', '7', '8', '9', '+', '/'
		};

		static char Tiny_Base64_Decode_Table[256] =
		{
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1,  0, -1, -1,
			-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
			15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
			-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
			41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
		};

		class Base64
		{
		public:
			static std::string Encode(const std::string & value)
			{
				std::string res;

				std::size_t size = value.size();

				const unsigned char * currentPtr = (unsigned char *)value.c_str();

				while (size > 2)
				{
					res += Tiny_Base64_Encode_Table[                                 currentPtr[0] >> 2 ];
					res += Tiny_Base64_Encode_Table[((currentPtr[0] & 0x03) << 4) + (currentPtr[1] >> 4)];
					res += Tiny_Base64_Encode_Table[((currentPtr[1] & 0x0f) << 2) + (currentPtr[2] >> 6)];
					res += Tiny_Base64_Encode_Table[  currentPtr[2] & 0x3f];

					size -= 3;
					currentPtr += 3;
				}

				if (size > 0)
				{
					res += Tiny_Base64_Encode_Table[currentPtr[0] >> 2];

					if (size % 3 == 1)
					{
						res += Tiny_Base64_Encode_Table[(currentPtr[0] & 0x03) << 4];
						res += "==";
					}
					else if (size % 3 == 2)
					{
						res += Tiny_Base64_Encode_Table[((currentPtr[0] & 0x03) << 4) + (currentPtr[1] >> 4)];
						res += Tiny_Base64_Encode_Table[ (currentPtr[1] & 0x0f) << 2];
						res += "=";
					}
				}

				return res;
			}

			static std::string EncodeS(const std::string & value)
			{
				std::string key;

				if (value.size() < 24)
				{
					while (key.size() != 24)
					{
						std::size_t size = 24 - key.size();

						key += value.substr(0, (size > value.size()) ? value.size() : size);
					}
				}
				else
				{
					key = value.substr(0, 24);
				}

				std::string valueEncode = Encode(value);

				for (char & valueIter : valueEncode)
				{
					for (char keyIter : key)
					{
						valueIter ^= keyIter;
					}
				}

				std::string keyEncode = Encode(key);

				for (char & valueIter : keyEncode)
				{
					for (char keyIter : Tiny_Base64_Encode_Table)
					{
						valueIter ^= keyIter;
					}
				}

				return TINY_STR_FORMAT("....{}{}", keyEncode, valueEncode);
			}

			static std::string Decode(const std::string & value)
			{
				int32_t i = 0;
				int32_t pos = 0;
				int32_t bin = 0;

				char ch;

				std::string res;

				const char * currentPtr = value.c_str();

				while ((ch = *currentPtr++) != '\0')
				{
					pos = i % 4;

					if (ch == '=')
					{
						/*
						 * 先说明一个概念 : 在解码时, 4个字符为一组进行一轮字符匹配.
						 *
						 * 两个条件 :
						 * 1、如果某一轮匹配的第二个是 "=" 且第三个字符不是 "=" , 说明这个带解析字符串不合法, 直接返回空
						 * 2、如果当前 "=" 不是第二个字符, 且后面的字符只包含空白符, 则说明这个这个条件合法, 可以继续.
						 *
						 */
						if (*currentPtr != '=' && pos == 1)
						{
							TINY_THROW_EXCEPTION(debug::ValueError, "decrypt data is invalid")

							return {};
						}

						continue;
					}

					ch = Tiny_Base64_Decode_Table[(unsigned char)ch];

					// 这个很重要，用来过滤所有不合法的字符
					if (ch < 0)
					{
						continue; /* a space or some other separator character, we simply skip over */
					}

					switch (pos)
					{
						case 0:
						{
							bin = ch << 2;

							break;
						}

						case 1:
						{
							bin |= ch >> 4;
							res += bin;
							bin = (ch & 0x0f) << 4;

							break;
						}

						case 2:
						{
							bin |= ch >> 2;
							res += bin;
							bin = (ch & 0x03) << 6;

							break;
						}

						case 3:
						{
							bin |= ch;
							res += bin;

							break;
						}

						default:
						{
							break;
						}
					}

					++i;
				}

				return res;
			}

			static std::string DecodeS(const std::string & value)
			{
				if (!TINY_STR_START_WITH(value,  "...."))
				{
					return value;
				}

				std::string str = value.substr(36);
				std::string key = value.substr(4, 32);

				for (char & valueIter : key)
				{
					for (char keyIter : Tiny_Base64_Encode_Table)
					{
						valueIter ^= keyIter;
					}
				}

				std::string keyDecode = Decode(key);

				for (char & valueIter : str)
				{
					for (char keyIter : keyDecode)
					{
						valueIter ^= keyIter;
					}
				}

				return Decode(str);
			}
		};
	}
}


#define TINY_BASE64_ENCODE(value) tinyCore::crypto::Base64::Encode(value)
#define TINY_BASE64_ENCODE_S(value) tinyCore::crypto::Base64::EncodeS(value)

#define TINY_BASE64_DECODE(value) tinyCore::crypto::Base64::Decode(value)
#define TINY_BASE64_DECODE_S(value) tinyCore::crypto::Base64::DecodeS(value)


#endif // __TINY_CORE__CRYPTO__BASE64__H__
