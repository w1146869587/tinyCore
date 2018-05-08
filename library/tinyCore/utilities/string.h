#ifndef __TINY_CORE__UTILITIES__STRING__H__
#define __TINY_CORE__UTILITIES__STRING__H__


/**
 *
 *  作者: hm
 *
 *  说明: 字符串处理
 *
 */


#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace utilities
	{
		class TINY_API String
		{
		public:
			static std::string & Trim(std::string & value)
			{
				static char const * spaceChars = "\n\r\t ";

				std::string::size_type end   = value.find_last_not_of(spaceChars);
				std::string::size_type start = value.find_first_not_of(spaceChars);

				value = (start == std::string::npos) ? "" : value.substr(start, 1 + end - start);

				return value;
			}

			static std::string TrimCopy(std::string const & value)
			{
				auto temp = value;

				return Trim(temp);
			}

			static std::string & ToLower(std::string & value)
			{
				std::transform(value.begin(), value.end(), value.begin(), ::tolower);

				return value;
			}

			static std::string ToLowerCopy(std::string const & value)
			{
				auto temp = value;

				return ToLower(temp);
			}

			static std::string & ToUpper(std::string & value)
			{
				std::transform(value.begin(), value.end(), value.begin(), ::toupper);

				return value;
			}

			static std::string ToUpperCopy(std::string const & value)
			{
				auto temp = value;

				return ToUpper(temp);
			}

			static std::string Filter(const std::string & value, const std::function<bool(char)> & func)
			{
				std::string result;

				for (char c: value)
				{
					if (func(c))
					{
						result.push_back(c);
					}
				}

				return result;
			}

			template <typename TypeT>
			static bool With(const std::string & value, const TypeT & infix)
			{
				return value.find(infix) != std::string::npos;
			}

			static bool StartWith(const std::string & value, const std::string & prefix)
			{
				return value.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), value.begin());
			}

			static bool EndWith(const std::string & value, const std::string & suffix)
			{
				return value.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
			}

			template <typename FileTypeT>
			static bool WriteFile(const FileTypeT & path, const char * value)
			{
				assert(value);

				std::ofstream oFile(path);

				if (oFile.is_open())
				{
					oFile << value << std::endl;

					oFile.close();

					return true;
				}

				return false;
			}

			template <typename FileTypeT>
			static bool WriteFile(const FileTypeT & path, const std::string & value)
			{
				std::ofstream oFile(path);

				if (oFile.is_open())
				{
					oFile << value << std::endl;

					oFile.close();

					return true;
				}

				return false;
			}

			template <typename FileTypeT>
			static bool WriteFile(const FileTypeT & path, const StringSet & value)
			{
				std::ofstream oFile(path);

				if (oFile.is_open())
				{
					for (auto &iter : value)
					{
						oFile << iter << std::endl;
					}

					oFile.close();

					return true;
				}

				return false;
			}

			template <typename FileTypeT>
			static bool WriteFile(const FileTypeT & path, const StringVector & value)
			{
				std::ofstream oFile(path);

				if (oFile.is_open())
				{
					for (auto &iter : value)
					{
						oFile << iter << std::endl;
					}

					oFile.close();

					return true;
				}

				return false;
			}

			static std::size_t Count(const std::string & value, char delimiter, std::size_t start = 0, std::size_t len = 0)
			{
				std::size_t count = 0;

				if (len == 0)
				{
					len = value.size();
				}
				else
				{
					len = std::min(start + len, value.size());
				}

				if (start >= len)
				{
					return 0;
				}

				while ((start = value.find(delimiter, start)) != std::string::npos)
				{
					if (start + 1 > len)
					{
						break;
					}

					++count;
					++start;
				}

				return count;
			}

			static std::size_t Count(const std::string & value, const char * key, std::size_t start = 0, std::size_t len = 0)
			{
				assert(key);

				return Count(value, key, strlen(key), start, len);
			}

			static std::size_t Count(const std::string & value, const std::string & key, std::size_t start = 0, std::size_t len = 0)
			{
				return Count(value, key.c_str(), key.size(), start, len);
			}

			static std::size_t Count(const std::string & value, const char * key, std::size_t keyLen, std::size_t start = 0, std::size_t len = 0)
			{
				std::size_t count = 0;

				std::string delimiter = key;

				if (keyLen == 0)
				{
					keyLen = delimiter.size();
				}
				else
				{
					delimiter.erase(keyLen);
				}

				if (len == 0)
				{
					len = value.size();
				}
				else
				{
					len = std::min(start + len, value.size());
				}

				if (start >= len)
				{
					return 0;
				}

				while ((start = value.find(delimiter, start)) != std::string::npos)
				{
					if (start + keyLen > len)
					{
						break;
					}

					count += 1;
					start += keyLen;
				}

				return count;
			}

			template <typename TypeT, typename KeyT>
			static std::size_t Split(const TypeT & value, const KeyT & key, StringSet & res, bool keepEnd = true)
			{
				std::size_t sub = 0;
				std::size_t pos = 0;

				std::string tempKey = key;
				std::string tempValue = value;

				if (keepEnd)
				{
					if (tempValue.find_last_of(tempKey) != (tempValue.size() - tempKey.size()))
					{
						tempValue += tempKey;
					}
				}

				while ((pos = tempValue.find(tempKey, pos)) != std::string::npos)
				{
					std::string str = tempValue.substr(sub, pos - sub);

					if (!str.empty())
					{
						res.insert(str);
					}

					pos += tempKey.size();
					sub  = pos;
				}

				if (res.empty())
				{
					res.insert(value);
				}

				return res.size();
			}

			template <typename TypeT, typename KeyT>
			static std::size_t Split(const TypeT & value, const KeyT & key, StringVector & res, bool keepEnd = true)
			{
				std::size_t sub = 0;
				std::size_t pos = 0;

				std::string tempKey = key;
				std::string tempValue = value;

				if (keepEnd)
				{
					if (tempValue.find_last_of(tempKey) != (tempValue.size() - tempKey.size()))
					{
						tempValue += tempKey;
					}
				}

				while ((pos = tempValue.find(tempKey, pos)) != std::string::npos)
				{
					std::string str = tempValue.substr(sub, pos - sub);

					if (!str.empty())
					{
						res.push_back(str);
					}

					pos += tempKey.size();
					sub  = pos;
				}

				if (res.empty())
				{
					res.push_back(value);
				}

				return res.size();
			}

			static std::size_t SplitLines(const std::string & value, StringSet & res, bool keepEnter = false)
			{
				std::size_t i = 0;
				std::size_t j = 0;
				std::size_t length = value.size();

				while (i < length)
				{
					while (i < length && value[i] != '\r' && value[i] != '\n' && value[i] != '\0')
					{
						++i;
					}

					std::size_t eol = i;

					if (i < length)
					{
						i += (value[i] == '\r' && i + 1 < length && value[i + 1] == '\n') ? 2 : 1;

						if (keepEnter)
						{
							eol = i;
						}
					}

					res.insert(value.substr(j, eol - j));

					j = i;
				}

				return res.size();
			}

			static std::size_t SplitLines(const std::string & value, StringVector & res, bool keepEnter = false)
			{
				std::size_t i = 0;
				std::size_t j = 0;
				std::size_t length = value.size();

				while (i < length)
				{
					while (i < length && value[i] != '\r' && value[i] != '\n' && value[i] != '\0')
					{
						++i;
					}

					std::size_t eol = i;

					if (i < length)
					{
						i += (value[i] == '\r' && i + 1 < length && value[i + 1] == '\n') ? 2 : 1;

						if (keepEnter)
						{
							eol = i;
						}
					}

					res.push_back(value.substr(j, eol - j));

					j = i;
				}

				return res.size();
			}

			template <typename TypeT, typename... Args>
			static std::string Format(const TypeT & fmt, Args &&... args)
			{
				return fmt::format(fmt, std::forward<Args>(args)...);
			}

			template<typename TypeT>
			static std::string & Erase(std::string & value, const TypeT & key, std::size_t keyLen = 0)
			{
				std::size_t pos  = 0;

				std::string delimiter = key;

				if (keyLen == 0)
				{
					keyLen = delimiter.size();
				}
				else
				{
					delimiter.erase(keyLen);
				}

				while ((pos = value.find(delimiter, pos)) != std::string::npos)
				{
					value.erase(pos, keyLen);
				}

				return value;
			}

			template <typename SrcTypeT, typename DstTypeT>
			static std::string & Replace(std::string & value, const SrcTypeT & src, const DstTypeT & dst)
			{
				return Replace(value, src, 0, dst, 0);
			};

			template <typename SrcTypeT, typename DstTypeT>
			static std::string & Replace(std::string & value, const SrcTypeT & src, std::size_t srcLen, const DstTypeT & dst, std::size_t dstLen)
			{
				std::string tempSrc = src;
				std::string tempDst = dst;

				if (srcLen == 0)
				{
					srcLen = tempSrc.size();
				}
				else
				{
					tempSrc.erase(srcLen);
				}

				if (dstLen == 0)
				{
					dstLen = tempDst.size();
				}
				else
				{
					tempDst.erase(dstLen);
				}

				for (std::size_t pos = 0; pos != std::string::npos ; pos += dstLen)
				{
					if ((pos = value.find(tempSrc, pos)) != std::string::npos)
					{
						value.replace(pos, srcLen, tempDst);
					}
					else
					{
						break;
					}
				}

				return value;
			}

			static bool ToBool(const std::string & value)
			{
				std::string str = ToLowerCopy(value);

				return str == "1" || str == "y" || str == "true" || str == "yes" || str == "on";
			}

			template <typename ValueT = int32_t, typename TypeT>
			static ValueT ToDigital(TypeT && value)
			{
				return boost::lexical_cast<ValueT>(std::forward<TypeT>(value));
			}

			template<typename TypeT>
			static std::string ToLocal(TypeT && value)
			{
				static std::locale loc("");

				std::stringstream ss;

				ss.imbue(loc);

				ss << value;

				return ss.str();
			}

			static std::string ToLocal(const double value, const int32_t bit = 1)
			{
				static std::locale loc("");

				std::stringstream ss;

				ss.imbue(loc);

				ss << std::fixed << std::setprecision(bit) << value;

				return ss.str();
			}

			template <typename TypeT>
			static std::string ToString(TypeT && value)
			{
				return boost::lexical_cast<std::string>(std::forward<TypeT>(value));
			}

			template <typename TypeT, typename... Args>
			static std::string ToString(TypeT && value, Args &&... args)
			{
				return ToString(std::forward<TypeT>(value)) + ToString(std::forward<Args>(args)...);
			}

			static std::string ToHexString(const char * value, bool reverse = false)
			{
				assert(value);

				return ToHexString(value, strlen(value), reverse);
			}

			static std::string ToHexString(const std::string & value, bool reverse = false)
			{
				return ToHexString(value, value.size(), reverse);
			}

			template <typename TypeT>
			static std::string ToHexString(const TypeT & value, std::size_t size, bool reverse = false)
			{
				long op   = reverse ? -1 : 1;
				long init = reverse ? size - 1 : 0;
				long end  = reverse ? -1 : (long)size;

				fmt::MemoryWriter out;

				for (long i = init; i != end; i += op)
				{
					out << fmt::hex(value[i]);
				}

				return out.str();
			}

			static ByteVector ToByteVector(const char * value)
			{
				assert(value);

				return ByteVector((const Byte *)value, (const Byte *)value + strlen(value));
			}

			static ByteVector ToByteVector(const std::string & value)
			{
				return ByteVector((const Byte *)value.data(), (const Byte *)value.data() + value.size());
			}

			static std::string fromJson(const rapidjson::Document & doc)
			{
				rapidjson::StringBuffer buffer;

				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

				doc.Accept(writer);

				return buffer.GetString();
			}
		};

		class HexString
		{
			template <typename ValueT>
			struct HexToDigital
			{
			public:
				operator ValueT() const
				{
					return value;
				}

				friend std::istream & operator>>(std::istream & in, HexToDigital & out)
				{
					in >> std::hex >> out.value;

					return in;
				}

			protected:
				ValueT value;
			};

		public:
			template <typename ValueT = uint32_t, typename TypeT>
			static ValueT ToDigital(TypeT && value)
			{
				return boost::lexical_cast<HexToDigital<ValueT>>(value);
			}

			static std::string ToString(const char * value, bool reverse = false)
			{
				assert(value);

				return ToString(value, strlen(value), reverse);
			}

			static std::string ToString(const std::string & value, bool reverse = false)
			{
				return ToString(value, value.size(), reverse);
			}

			template <typename TypeT>
			static std::string ToString(const TypeT & value, std::size_t size, bool reverse = false)
			{
				long op   = reverse ? -2 : 2;
				long init = reverse ? size - 2 : 0;
				long end  = reverse ? -2 : (long)size;

				std::string tempStr;

				for (long i = init; i != end; i += op)
				{
					char str[3] = { value[i], value[i + 1], '\0' };

					tempStr.push_back((char)ToDigital(str));
				}

				return tempStr;
			}
		};
	}
}


#define TINY_STR_TO_BOOL(value) tinyCore::utilities::String::ToBool(value)
#define TINY_STR_TO_BYTE_VECTOR(value) tinyCore::utilities::String::ToByteVector(value)
#define TINY_STR_TRIM(value) tinyCore::utilities::String::Trim(value)
#define TINY_STR_TO_LOWER(value) tinyCore::utilities::String::ToLower(value)
#define TINY_STR_TO_UPPER(value) tinyCore::utilities::String::ToUpper(value)
#define TINY_STR_TRIM_COPY(value) tinyCore::utilities::String::TrimCopy(value)
#define TINY_STR_TO_LOWER_COPY(value) tinyCore::utilities::String::ToLowerCopy(value)
#define TINY_STR_TO_UPPER_COPY(value) tinyCore::utilities::String::ToUpperCopy(value)

#define TINY_STR_WITH(value, infix) tinyCore::utilities::String::With(value, infix)
#define TINY_STR_END_WITH(value, suffix) tinyCore::utilities::String::EndWith(value, suffix)
#define TINY_STR_WRITE_FILE(path, value) tinyCore::utilities::String::WriteFile(path, value)
#define TINY_STR_START_WITH(value, prefix) tinyCore::utilities::String::StartWith(value, prefix)
#define TINY_STR_TO_DIGITAL(type, value) tinyCore::utilities::String::ToDigital<type>(value)

#define TINY_STR_FORMAT(fmt, ...) tinyCore::utilities::String::Format(fmt, ##__VA_ARGS__)
#define TINY_STR_CFORMAT(fmt, ...) tinyCore::utilities::String::Format(fmt, ##__VA_ARGS__).data()
#define TINY_STR_TO_LOCAL(value, ...) tinyCore::utilities::String::ToLocal(value, ##__VA_ARGS__)
#define TINY_STR_TO_STRING(value, ...) tinyCore::utilities::String::ToString(value, ##__VA_ARGS__)
#define TINY_STR_TO_CSTRING(value, ...) tinyCore::utilities::String::ToString(value, ##__VA_ARGS__).data()
#define TINY_STR_TO_HEX_STRING(value, ...) tinyCore::utilities::String::ToHexString(value, ##__VA_ARGS__)

#define TINY_STR_ERASE(value, key, ...) tinyCore::utilities::String::Erase(value, key, ##__VA_ARGS__)
#define TINY_STR_COUNT(value, key, ...) tinyCore::utilities::String::Count(value, key, ##__VA_ARGS__)
#define TINY_STR_REPLACE(value, src, ...) tinyCore::utilities::String::Replace(value, src, ##__VA_ARGS__)
#define TINY_STR_SPLIT(value, key, res, ...) tinyCore::utilities::String::Split(value, key, res, ##__VA_ARGS__)
#define TINY_STR_SPLIT_LINES(value, res, ...) tinyCore::utilities::String::SplitLines(value, res, ##__VA_ARGS__)

#define TINY_HEX_STR_TO_DIGITAL(type, value) tinyCore::utilities::HexString::ToDigital<type>(value)

#define TINY_HEX_STR_TO_STRING(value, ...) tinyCore::utilities::HexString::ToString(value, ##__VA_ARGS__)
#define TINY_HEX_STR_TO_CSTRING(value, ...) tinyCore::utilities::HexString::ToString(value, ##__VA_ARGS__).data()


#endif // __TINY_CORE__UTILITIES__STRING__H__
