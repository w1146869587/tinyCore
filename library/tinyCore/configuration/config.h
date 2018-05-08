#ifndef __TINY_CORE__CONFIGURATION__CONFIG__H__
#define __TINY_CORE__CONFIGURATION__CONFIG__H__


/**
 *
 *  作者: hm
 *
 *  说明: 配置解析
 *
 */


#include <tinyCore/utilities/time.h>
#include <tinyCore/system/fileSystem.h>


namespace tinyCore
{
	namespace configuration
	{
		class JsonConfig
		{
		public:
			JsonConfig() = default;

			explicit JsonConfig(const char * path) : _path(path)
			{

			}

			explicit JsonConfig(std::string path) : _path(std::move(path))
			{

			}

			bool IsModify()
			{
				if (!TINY_FILE_IS_EXISTS(_path))
				{
					_error = TINY_STR_FORMAT("无法找到配置文件 ({})", _path);

					return false;
				}

				auto time = TINY_TIME_SECONDS(TINY_FILE_LAST_WRITE_TIME(_path));

				if (time == _lastTime)
				{
					return false;
				}
				else
				{
					_lastTime = time;

					return true;
				}
			}

			bool Initialize()
			{
				std::ifstream file;

				file.open(_path, std::ios::binary);

				if (!file.is_open())
				{
					_error = TINY_STR_FORMAT("无法打开配置文件 ({})", _path);

					return false;
				}

				std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

				file.close();

				_content = str;

				rapidjson::Document temp = { };

				temp.Parse(str.c_str());

				if (temp.HasParseError())
				{
					_error = TINY_STR_FORMAT("配置文件 ({}) 解析错误 : {}({})",
											 _path,
											 rapidjson::GetParseError_En(temp.GetParseError()),
											 temp.GetErrorOffset());

					return false;
				}

				document = std::move(temp);

				return true;
			}

			void SetConfigPath(std::string path)
			{
				_path = std::move(path);
			}

			template<typename TypeT>
			void SetConfigPath(const TypeT & path)
			{
				_path = path;
			}

			std::string LastWriteTime() const
			{
				return TINY_TIME_FORMAT_TIME_STRING(_lastTime);
			}

			const std::string & Path() const
			{
				return _path;
			}

			const std::string & Content() const
			{
				return _content;
			}

			const std::string & LastError() const
			{
				return _error;
			}

			template <typename ValueT, typename CharType, size_t N>
			bool GetValue(ValueT * value, const CharType (&key)[N])
			{
				TINY_ASSERT(value, "value is nullptr")

				if (value == nullptr)
				{
					return false;
				}

				const rapidjson::Value * ptr = rapidjson::GetValueByPointer(document, key);

				if (ptr == nullptr)
				{
					_error = TINY_STR_FORMAT("Miss Field ({})", key);

					return false;
				}
				else if (!ptr->Is<ValueT>())
				{
					_error = TINY_STR_FORMAT("Field Type Invalid ({})", key);

					return false;
				}
				else
				{
					*value = ptr->Get<ValueT>();

					return true;
				}
			}

			template <typename CharType, size_t N>
			bool GetString(std::string & value, const CharType (&key)[N])
			{
				const rapidjson::Value * ptr = rapidjson::GetValueByPointer(document, key);

				if (ptr == nullptr)
				{
					_error = TINY_STR_FORMAT("Miss Field ({})", key);

					return false;
				}
				else if (!ptr->IsString())
				{
					_error = TINY_STR_FORMAT("Field Type Invalid ({})", key);

					return false;
				}
				else
				{
					value = ptr->GetString();

					return true;
				}
			}

		protected:
			time_t _lastTime{ 0 };

			std::string _path{ };
			std::string _error{ };
			std::string _content{ };

			rapidjson::Document document{ };
		};
	}
}


#endif // __TINY_CORE__CONFIGURATION__CONFIG__H__
