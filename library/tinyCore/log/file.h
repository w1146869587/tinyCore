#ifndef __TINY_CORE__LOG__FILE__H__
#define __TINY_CORE__LOG__FILE__H__


/**
 *
 *  作者: hm
 *
 *  说明: 日志文件
 *
 */


#include <tinyCore/log/detail.h>


namespace tinyCore
{
	namespace log
	{
		class LogFile
		{
		public:
			LogFile() = default;

			~LogFile()
			{
				Close();
			}

			void Close()
			{
				if (_file)
				{
					std::fflush(_file);
					std::fclose(_file);

					_size = 0;

					_file = nullptr;
				}
			}

			bool Flush()
			{
				TINY_ASSERT(IsOpen(), "File Not Open");

				return fflush(_file) > 0;
			}

			bool IsOpen() const
			{
				return _file != nullptr;
			}

			bool IsClose() const
			{
				return _file == nullptr;
			}

			void Open(const system::FileSystem::PathInfo & path, bool truncate = false)
			{
				TINY_ASSERT(IsClose(), "File Already Open");

				CheckPath(path);

				if (!system::FileSystem::IsExists(_path.parent_path()))
				{
					system::FileSystem::CreateDirectories(_path.parent_path());
				}

				_file = std::fopen(_path.c_str(), truncate ? "wb" : "ab");

				TINY_THROW_EXCEPTION_IF(IsClose(), debug::FileError, "Open File Error")
			}

			void Reopen(const system::FileSystem::PathInfo & path, bool truncate = false)
			{
				if (IsOpen())
				{
					Close();
				}

				Open(path, truncate);
			}

			void Write(const LogMessage & msg)
			{
				TINY_ASSERT(IsOpen(), "File Not Open");

				std::size_t size = msg.formatted.size();

				if (std::fwrite(msg.formatted.data(), 1, size, _file) != size)
				{
					TINY_THROW_EXCEPTION(debug::FileError, TINY_STR_FORMAT("Failed writing file {}", _path.string()))
				}
				else
				{
					_size += size;
				}
			}

			const std::size_t Size() const
			{
				return _size;
			}

			const system::FileSystem::PathInfo & Path() const
			{
				return _path;
			}

		protected:
			void CheckPath(const system::FileSystem::PathInfo & path)
			{
				if (path.empty())
				{
					_path = TINY_STR_FORMAT("{}/log/{}.log",
											system::FileSystem::ApplicationPath().parent_path().string(),
											system::FileSystem::ApplicationPath().filename().string());
				}
				else
				{
					if (path.is_absolute())
					{
						_path = path;
					}
					else
					{
						if (TINY_STR_START_WITH(path.string(), ".."))
						{
							_path = system::FileSystem::CanonicalPath(path.parent_path()) / path.filename();
						}
						else
						{
							_path = system::FileSystem::CurrentPath() / path;
						}
					}
				}
			}

		protected:
			FILE * _file{ nullptr };

			std::size_t _size{ 0 };

			system::FileSystem::PathInfo _path{ };
		};
	}
}


#endif // __TINY_CORE__LOG__FILE__H__
