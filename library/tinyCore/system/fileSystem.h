#ifndef __TINY_CORE__SYSTEM__FILE_SYSTEM__H__
#define __TINY_CORE__SYSTEM__FILE_SYSTEM__H__


/**
 *
 *  作者: hm
 *
 *  说明: 文件管理
 *
 */


#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace system
	{
		class FileSystem
		{
			/**
			 *
			 * /usr/sniffer/abc.txt
			 *
			 * stem()            abc                  不带扩展名的文件名
			 * filename()        abc.txt              文件名
			 * extension()       .txt                 扩展名
			 * root_name()                            根名
			 * root_path()       /                    根路径：根名+根目录
			 * parent_path()     /usr/sniffer         父路径：根路径+相对路径
			 * relative_path()   usr/sniffer/abc.txt  相对路径
			 * root_directory()  /                    根目录
			 *
			 */

		public:
			using PathInfo				= std::experimental::filesystem::path;
			using FileType				= std::experimental::filesystem::file_type;
			using TimeType				= std::experimental::filesystem::file_time_type;
			using SpaceInfo				= std::experimental::filesystem::space_info;
			using FileStatus			= std::experimental::filesystem::file_status;
			using CopyOptions			= std::experimental::filesystem::copy_options;
			using DirectoryIterator		= std::experimental::filesystem::directory_iterator;
			using PathInfoContainer		= std::set<FileSystem::PathInfo>;
			using PermissionsOptions	= std::experimental::filesystem::perms;

			static DirectoryIterator End()
			{
				return DirectoryIterator();
			}

			static DirectoryIterator Begin(const PathInfo & path)
			{
				return DirectoryIterator(path, _code);
			}

			template <typename TypeT>
			static bool IsEmpty(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::is_empty(path, _code);
			}

			template <typename TypeT>
			static bool IsFifo(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::is_fifo(path, _code);
			}

			static bool IsFifo(FileStatus status)
			{
				return std::experimental::filesystem::is_fifo(status);
			}

			template <typename TypeT>
			static bool IsOther(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::is_other(path, _code);
			}

			static bool IsOther(FileStatus status)
			{
				return std::experimental::filesystem::is_other(status);
			}

			template <typename TypeT>
			static bool IsExists(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::exists(path, _code);
			}

			static bool IsExists(FileStatus status)
			{
				return std::experimental::filesystem::exists(status);
			}

			template <typename TypeT>
			static bool IsSocket(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::is_socket(path, _code);
			}

			static bool IsSocket(FileStatus status)
			{
				return std::experimental::filesystem::is_socket(status);
			}

			template <typename TypeT>
			static bool IsSymlink(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::is_symlink(path, _code);
			}

			static bool IsSymlink(FileStatus status)
			{
				return std::experimental::filesystem::is_symlink(status);
			}

			template <typename TypeT>
			static bool IsDirectory(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::is_directory(path, _code);
			}

			static bool IsDirectory(FileStatus status)
			{
				return std::experimental::filesystem::is_directory(status);
			}

			template <typename TypeT>
			static bool IsBlockFile(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::is_block_file(path, _code);
			}

			static bool IsBlockFile(FileStatus status)
			{
				return std::experimental::filesystem::is_block_file(status);
			}

			template <typename TypeT>
			static bool IsCharacterFile(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::is_character_file(path, _code);
			}

			static bool IsCharacterFile(FileStatus status)
			{
				return std::experimental::filesystem::is_character_file(status);
			}

			template <typename TypeT>
			static bool IsRegularFile(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::is_regular_file(path, _code);
			}

			static bool IsRegularFile(FileStatus status)
			{
				return std::experimental::filesystem::is_regular_file(status);
			}

			template <typename TypeT1, typename TypeT2>
			static bool IsEquivalent(TypeT1 && sDir, TypeT2 && dDir)
			{
				FileSystem::PathInfo sPath(std::forward<TypeT1>(sDir));
				FileSystem::PathInfo dPath(std::forward<TypeT2>(dDir));

				return std::experimental::filesystem::equivalent(sPath, dPath, _code);
			}

			template <typename TypeT1, typename TypeT2>
			static void Rename(TypeT1 && sDir, TypeT2 && dDir)
			{
				FileSystem::PathInfo sPath(std::forward<TypeT1>(sDir));
				FileSystem::PathInfo dPath(std::forward<TypeT2>(dDir));

				std::experimental::filesystem::rename(sPath, dPath, _code);
			}

			template <typename TypeT>
			static bool CreateFile(const TypeT & dir)
			{
				std::ofstream file(dir, std::ios::out | std::ios::binary);

				if (file.is_open())
				{
					file.close();

					return true;
				}

				return false;
			}

			template <typename TypeT>
			static bool CreateDirectory(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::create_directory(path, _code);
			}

			template <typename TypeT1, typename TypeT2>
			static bool CreateDirectory(TypeT1 && dir, TypeT2 && attributes)
			{
				FileSystem::PathInfo path(std::forward<TypeT1>(dir));
				FileSystem::PathInfo attributesPath(std::forward<TypeT2>(attributes));

				return std::experimental::filesystem::create_directory(path, attributesPath, _code);
			}

			template <typename TypeT>
			static bool CreateDirectories(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::create_directories(path, _code);
			}

			template <typename TypeT>
			static void TraverseFile(TypeT && dir, PathInfoContainer & res)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				if (!FileSystem::IsExists(path))
				{
					return;
				}

				for (auto &iter : DirectoryIterator(path))
				{
					if (FileSystem::IsRegularFile(iter.status(_code)))
					{
						res.insert(iter.path());
					}
					else if (FileSystem::IsDirectory(iter.status(_code)))
					{
						TraverseFile(iter.path(), res);
					}
				}
			}

			template <typename TypeT, typename TypeR>
			static void TraverseRegexFile(TypeT && dir, PathInfoContainer & res, const TypeR & rule)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				if (!FileSystem::IsExists(path))
				{
					return;
				}

				auto regexStr = std::regex(rule);

				for (auto &iter : DirectoryIterator(path))
				{
					if (FileSystem::IsRegularFile(iter.status(_code)))
					{
						if (std::regex_match(iter.path().filename().string(), regexStr))
						{
							res.insert(iter.path());
						}
					}
					else if (FileSystem::IsDirectory(iter.status(_code)))
					{
						TraverseRegexFile(iter.path(), res, rule);
					}
				}
			}

			template <typename TypeT>
			static void TraverseDirectory(TypeT && dir, PathInfoContainer & res)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				if (!FileSystem::IsExists(path))
				{
					return;
				}

				for (auto &iter : DirectoryIterator(path))
				{
					if (FileSystem::IsDirectory(iter.status(_code)))
					{
						res.insert(iter.path());

						TraverseDirectory(iter.path(), res);
					}
				}
			}

			template <typename TypeT, typename TypeR>
			static void TraverseRegexDirectory(TypeT && dir, PathInfoContainer & res, const TypeR & rule)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				if (!FileSystem::IsExists(path))
				{
					return;
				}

				auto regexStr = std::regex(rule);

				for (auto &iter : DirectoryIterator(path))
				{
					if (FileSystem::IsDirectory(iter.status(_code)))
					{
						if (std::regex_match(iter.path().filename().string(), regexStr))
						{
							res.insert(iter.path());
						}

						TraverseRegexDirectory(iter.path(), res, rule);
					}
				}
			}

			template <typename TypeT>
			static void TraverseAll(TypeT && dir, PathInfoContainer & fileRes, PathInfoContainer & directoryRes)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				if (!FileSystem::IsExists(path))
				{
					return;
				}

				auto iend = FileSystem::End();
				auto itor = FileSystem::Begin(path);

				for (; itor != iend; ++itor)
				{
					if (FileSystem::IsRegularFile(itor->status(_code)))
					{
						fileRes.insert(itor->path());
					}
					else if (FileSystem::IsDirectory(itor->status(_code)))
					{
						directoryRes.insert(itor->path());

						TraverseAll(itor->path(), fileRes, directoryRes);
					}
				}
			}

			template <typename TypeT>
			static void ResizeFile(TypeT && dir, const std::size_t size)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				std::experimental::filesystem::resize_file(path, size, _code);
			}

			// 访问权限
			template <typename TypeT>
			static void Permissions(TypeT && dir, PermissionsOptions type = PermissionsOptions::all)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				std::experimental::filesystem::permissions(path, type, _code);
			}

			template <typename TypeT>
			static bool Remove(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::remove(path, _code);
			}

			template <typename TypeT>
			static bool RemoveAll(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::remove_all(path, _code) > 0;
			}

			template <typename TypeT1, typename TypeT2>
			static void Copy(TypeT1 && sDir, TypeT2 && dDir, CopyOptions options = CopyOptions::none)
			{
				FileSystem::PathInfo sPath(std::forward<TypeT1>(sDir));
				FileSystem::PathInfo dPath(std::forward<TypeT2>(dDir));

				std::experimental::filesystem::copy(sPath, dPath, options, _code);
			}

			template <typename TypeT1, typename TypeT2>
			static void CopyFile(TypeT1 && sDir, TypeT2 && dDir)
			{
				FileSystem::PathInfo sPath(std::forward<TypeT1>(sDir));
				FileSystem::PathInfo dPath(std::forward<TypeT2>(dDir));

				std::experimental::filesystem::copy_file(sPath, dPath, _code);
			}

			template <typename TypeT1, typename TypeT2>
			static void CopySymlink(TypeT1 && sDir, TypeT2 && dDir)
			{
				FileSystem::PathInfo sPath(std::forward<TypeT1>(sDir));
				FileSystem::PathInfo dPath(std::forward<TypeT2>(dDir));

				std::experimental::filesystem::copy_symlink(sPath, dPath, _code);
			}

			template <typename TypeT1, typename TypeT2>
			static void CreateSymlink(TypeT1 && sDir, TypeT2 && dDir)
			{
				FileSystem::PathInfo sPath(std::forward<TypeT1>(sDir));
				FileSystem::PathInfo dPath(std::forward<TypeT2>(dDir));

				std::experimental::filesystem::create_symlink(sPath, dPath, _code);
			}

			template <typename TypeT1, typename TypeT2>
			static void CreateHardLink(TypeT1 && sDir, TypeT2 && dDir)
			{
				FileSystem::PathInfo sPath(std::forward<TypeT1>(sDir));
				FileSystem::PathInfo dPath(std::forward<TypeT2>(dDir));

				std::experimental::filesystem::create_hard_link(sPath, dPath, _code);
			}

			template <typename TypeT1, typename TypeT2>
			static void CreateDirectorySymlink(TypeT1 && sDir, TypeT2 && dDir)
			{
				FileSystem::PathInfo sPath(std::forward<TypeT1>(sDir));
				FileSystem::PathInfo dPath(std::forward<TypeT2>(dDir));

				std::experimental::filesystem::create_directory_symlink(sPath, dPath, _code);
			}

			template <typename TypeT>
			static void ModifyCurrentPath(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				std::experimental::filesystem::current_path(path, _code);
			}

			template <typename TypeT>
			static void ModifyLastWriteTime(TypeT && dir, TimeType times = std::chrono::system_clock::now())
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				std::experimental::filesystem::last_write_time(path, times, _code);
			}

			template <typename TypeT>
			static std::size_t FileSize(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::file_size(path, _code);
			}

			template <typename TypeT>
			static std::size_t HardLinkCount(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::hard_link_count(path, _code);
			}

			template <typename TypeT>
			static TimeType LastWriteTime(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::last_write_time(path, _code);
			}

			/**
			 *
			 * 获得符号链接的目标
			 *
			 * /lib/libc.so.6 --> libc-2.17.so
			 *
			 */
			template <typename TypeT>
			static PathInfo ReadSymlink(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::read_symlink(path, _code);
			}

			/**
			 *
			 * 临时目录
			 *
			 * /tmp
			 *
			 */
			static PathInfo TempPath()
			{
				return std::experimental::filesystem::temp_directory_path(_code);
			}

			/**
			 *
			 * 当前路径
			 *
			 * /home/code/c++/tinyCore/cmake-build-debug/bin
			 *
			 */
			static PathInfo CurrentPath()
			{
				return std::experimental::filesystem::current_path(_code);
			}

			/**
			 *
			 * 规范绝对路径
			 *
			 * ../../ --> /home/code/c++/tinyCore
			 *
			 */
			template <typename TypeT>
			static PathInfo CanonicalPath(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::canonical(path, _code);
			}

			/**
			 *
			 * 绝对路径
			 *
			 * ../../ --> /home/code/c++/tinyCore/cmake-build-debug/bin/../../
			 *
			 */
			template <typename TypeT>
			static PathInfo AbsolutePath(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::absolute(path);
			}

			/**
			 *
			 * 完整路径(当前路径 + 相对路径)
			 *
			 * ../../ --> /home/code/c++/tinyCore/cmake-build-debug/bin/../../
			 *
			 */
			template <typename TypeT>
			static PathInfo SystemCompletePath(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::system_complete(path, _code);
			}

			// 文件系统信息
			template <typename TypeT>
			static SpaceInfo Space(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::space(path, _code);
			}

			template <typename TypeT>
			static FileStatus Status(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::status(path, _code);
			}

			template <typename TypeT>
			static FileStatus SymlinkStatus(TypeT && dir)
			{
				FileSystem::PathInfo path(std::forward<TypeT>(dir));

				return std::experimental::filesystem::symlink_status(path, _code);
			}

			static PathInfo ApplicationPath()
			{
				static PathInfo path;

				if (path.empty())
				{
					char str[1024] = { 0 };

					ssize_t count = readlink("/proc/self/exe", str, sizeof(str) - 1);

					if (count <= 0 || count > (ssize_t)(sizeof(str) - 1))
					{
						TINY_THROW_EXCEPTION(debug::IllegalArguments, TINY_STR_FORMAT("readlink : {}", strerror(errno)))
					}

					path.assign(str);
				}

				return path;
			}

			static PathInfo ApplicationName()
			{
				static PathInfo name = ApplicationPath().stem();

				return name;
			}

			static PathInfo ApplicationParentPath()
			{
				static PathInfo path = ApplicationPath().parent_path();

				return path;
			}

			template <typename TypeT>
			static std::size_t Lines(const TypeT & path)
			{
				std::ifstream ifs(path);

				TINY_THROW_EXCEPTION_IF(!ifs.is_open(), debug::FileError, TINY_STR_FORMAT("Failed open file {}", path))

				std::string line;

				size_t counter = 0;

				while (std::getline(ifs, line))
				{
					counter++;
				}

				return counter;
			}

			template <typename TypeT>
			static std::string Contents(const TypeT & path)
			{
				std::ifstream ifs(path);

				TINY_THROW_EXCEPTION_IF(!ifs.is_open(), debug::FileError, TINY_STR_FORMAT("Failed open file {}", path))

				return std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
			}

			static std::error_code & LastError()
			{
				return _code;
			}

		protected:
			static std::error_code _code;
		};
	}
}


#define TINY_FILE_TEMP_PATH() tinyCore::system::FileSystem::TempPath()
#define TINY_FILE_LAST_ERROR() tinyCore::system::FileSystem::LastError()
#define TINY_FILE_CURRENT_PATH() tinyCore::system::FileSystem::CurrentPath()
#define TINY_FILE_APPLICATION_PATH() tinyCore::system::FileSystem::ApplicationPath()
#define TINY_FILE_APPLICATION_NAME() tinyCore::system::FileSystem::ApplicationName()
#define TINY_FILE_APPLICATION_PARENT_PATH() tinyCore::system::FileSystem::ApplicationParentPath()

#define TINY_FILE_IS_FIFO(file) tinyCore::system::FileSystem::IsFifo(file)
#define TINY_FILE_IS_EMPTY(file) tinyCore::system::FileSystem::IsEmpty(file)
#define TINY_FILE_IS_OTHER(file) tinyCore::system::FileSystem::IsOther(file)
#define TINY_FILE_IS_EXISTS(file) tinyCore::system::FileSystem::IsExists(file)
#define TINY_FILE_IS_SOCKET(file) tinyCore::system::FileSystem::IsSocket(file)
#define TINY_FILE_IS_DIRECTORY(file) tinyCore::system::FileSystem::IsDirectory(file)
#define TINY_FILE_IS_SYSTEM_LINK(file) tinyCore::system::FileSystem::IsSymlink(file)
#define TINY_FILE_IS_BLOCK_FILE(file) tinyCore::system::FileSystem::IsBlockFile(file)
#define TINY_FILE_IS_REGULAR_FILE(file) tinyCore::system::FileSystem::IsRegularFile(file)
#define TINY_FILE_IS_CHARACTER_FILE(file) tinyCore::system::FileSystem::IsCharacterFile(file)
#define TINY_FILE_IS_EQUIVALENT(src, dst) tinyCore::system::FileSystem::IsEquivalent(src, dst)

#define TINY_FILE_SIZE(file) tinyCore::system::FileSystem::FileSize(file)
#define TINY_FILE_SPACE(file) tinyCore::system::FileSystem::Space(file)
#define TINY_FILE_LINES(file) tinyCore::system::FileSystem::Lines(file)
#define TINY_FILE_STATUS(file) tinyCore::system::FileSystem::Status(file)
#define TINY_FILE_REMOVE(file) tinyCore::system::FileSystem::Remove(file)
#define TINY_FILE_CONTENTS(file) tinyCore::system::FileSystem::Contents(file)
#define TINY_FILE_REMOVE_ALL(file) tinyCore::system::FileSystem::RemoveAll(file)
#define TINY_FILE_ABSOLUTE_PATH(file) tinyCore::system::FileSystem::AbsolutePath(file)
#define TINY_FILE_CANONICAL_PATH(file) tinyCore::system::FileSystem::CanonicalPath(file)
#define TINY_FILE_HARD_LINK_COUNT(file) tinyCore::system::FileSystem::HardLinkCount(file)
#define TINY_FILE_LAST_WRITE_TIME(file) tinyCore::system::FileSystem::LastWriteTime(file)
#define TINY_FILE_READ_SYSTEM_LINK(file) tinyCore::system::FileSystem::ReadSymlink(file)
#define TINY_FILE_CREATE_FILE(file) tinyCore::system::FileSystem::CreateFile(file)
#define TINY_FILE_CREATE_DIRECTORY(file) tinyCore::system::FileSystem::CreateDirectory(file)
#define TINY_FILE_SYSTEM_LINK_STATUS(file) tinyCore::system::FileSystem::SymlinkStatus(file)
#define TINY_FILE_CREATE_DIRECTORIES(file) tinyCore::system::FileSystem::CreateDirectories(file)
#define TINY_FILE_MODIFY_CURRENT_PATH(file) tinyCore::system::FileSystem::ModifyCurrentPath(file)
#define TINY_FILE_SYSTEM_COMPLETE_PATH(file) tinyCore::system::FileSystem::SystemCompletePath(file)

#define TINY_FILE_RENAME(file, res) tinyCore::system::FileSystem::Rename(file, res)
#define TINY_FILE_RESIZE(file, size) tinyCore::system::FileSystem::ResizeFile(file, size)
#define TINY_FILE_COPY_FILE(src, dst) tinyCore::system::FileSystem::CopyFile(src, dst)
#define TINY_FILE_TRAVERSE_FILE(file, res) tinyCore::system::FileSystem::TraverseFile(file, res)
#define TINY_FILE_COPY_SYSTEM_LINK(src, dst) tinyCore::system::FileSystem::CopySymlink(src, dst)
#define TINY_FILE_CREATE_HARD_LINK(src, dst) tinyCore::system::FileSystem::CreateHardLink(src, dst)
#define TINY_FILE_CREATE_SYSTEM_LINK(src, dst) tinyCore::system::FileSystem::CreateSymlink(src, dst)
#define TINY_FILE_TRAVERSE_DIRECTORY(file, res) tinyCore::system::FileSystem::TraverseDirectory(file, res)
#define TINY_FILE_CREATE_DIRECTORY_SYMLINK(src, dst) tinyCore::system::FileSystem::CreateDirectorySymlink(src, dst)

#define TINY_FILE_TRAVERSE_ALL(file, fileRes, dirRes) tinyCore::system::FileSystem::TraverseDirectory(file, fileRes, dirRes)
#define TINY_FILE_TRAVERSE_REGEX_FILE(file, res, rule) tinyCore::system::FileSystem::TraverseRegexFile(file, res, rule)
#define TINY_FILE_TRAVERSE_REGEX_DIRECTORY(file, res, rule) tinyCore::system::FileSystem::TraverseRegexDirectory(file, res, rule)

#define TINY_FILE_PERMISSIONS(file, ...) tinyCore::system::FileSystem::Permissions(file, ##__VA_ARGS__)
#define TINY_FILE_MODIFY_LAST_WRITE_TIME(file, ...) tinyCore::system::FileSystem::ModifyLastWriteTime(file, ##__VA_ARGS__)

#define TINY_FILE_COPY(src, dst, ...) tinyCore::system::FileSystem::Copy(src, dst, ##__VA_ARGS__)


#endif // __TINY_CORE__SYSTEM__FILE_SYSTEM__H__
