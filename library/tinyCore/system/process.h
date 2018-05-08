#ifndef __TINY_CORE__SYSTEM__PROCESS__H__
#define __TINY_CORE__SYSTEM__PROCESS__H__


/**
 *
 *  作者: hm
 *
 *  说明: 进程管理
 *
 */


#include <tinyCore/system/fileSystem.h>


namespace tinyCore
{
	namespace system
	{
		class Process
		{
		public:
			static bool IsRunning(const char * path = TINY_FILE_APPLICATION_NAME().c_str())
			{
				TINY_ASSERT(path, "path is nullptr")

				int32_t fd = open(TINY_STR_CFORMAT("/var/run/{}.pid", path), O_WRONLY | O_CREAT, 0644);

				if (fd < 0)
				{
					TINY_WARNING(strerror(errno));

					return false;
				}

				if (Flock(fd, F_SETLK, F_WRLCK, SEEK_SET, 0, 0) == -1)  // try and set a write lock on the entire file
				{
					close(fd);

					if (errno == EACCES || errno == EAGAIN)
					{
						return true;  // is running
					}

					TINY_WARNING(strerror(errno));

					exit(1);
				}

				/*
				 * int ftruncate(int fd, off_t length)
				 *
				 * 将参数fd指定的文件大小改为参数length指定的大小
				 *
				 * fd 为已打开的文件描述词, 而且必须是以写入模式打开的文件
				 *
				 * 如果原来的文件大小比参数length大，则超过的部分会被删去
				 *
				 */
				if (ftruncate(fd, 0) == -1)  // truncate to zero length, now that we have the lock
				{
					TINY_WARNING(strerror(errno));

					close(fd);

					exit(1);
				}

				std::string pidStr = TINY_STR_FORMAT("{}\n", getpid());

				if (write(fd, pidStr.c_str(), pidStr.size()) != (ssize_t)pidStr.size())  // write our process id
				{
					TINY_WARNING(strerror(errno));

					close(fd);

					exit(1);
				}

				int32_t val = fcntl(fd, F_GETFD, 0);

				if (val == -1)  // set close-on-exec flag for descriptor
				{
					TINY_WARNING(strerror(errno));

					close(fd);

					exit(1);
				}

				val |= FD_CLOEXEC;

				if (fcntl(fd, F_SETFD, val) == -1)
				{
					close(fd);

					TINY_WARNING(strerror(errno));

					exit(1);
				}

				return false;  // leave file open until we terminate: lock will be held
			}

			static void Fork(bool isChangeDir = true, bool isCloseIO = true)
			{
				pid_t pid = ::fork();

				if (pid < 0)  // in case of fork is error
				{
					TINY_WARNING(strerror(errno));

					return;
				}

				if (pid != 0)  // in case of this is parent process
				{
					exit(0);
				}

				/*
				 * 当进程是会话的领头进程时setsid()调用失败并返回(-1)
				 *
				 * setsid()调用成功后, 返回新的会话的ID, 调用setsid函数的进程成为新的会话的领头进程, 并与其父进程的会话组和进程组脱离
				 *
				 * 由于会话对控制终端的独占性, 进程同时与控制终端脱离
				 *
				 *
				 * 之前parent和child运行在同一个session里, parent是会话(session)的领头进程
				 *
				 * 所以作为session头的parent如果exit结束执行的话,那么会话session组中的所有进程将都被杀死
				 *
				 * 执行setsid()之后, child将重新获得一个新的会话(session)id
				 *
				 * 这时parent退出之后,将不会影响到child了
				 *
				 */
				pid = setsid();

				if (pid < -1)
				{
					TINY_WARNING(strerror(errno));

					return;
				}

				if (isChangeDir)  // 更改当前工作目录为根目录
				{
					chdir("/");
				}

				if (isCloseIO)  // 关闭文件描述符
				{
					int32_t fd = open("/dev/null", O_RDWR, 0);

					if (fd != -1)
					{
						/*
						 * int dup2(int oldHandle,int newHandle)
						 *
						 * 复制文件句柄
						 *
						 * #define STDIN_FILENO 0
						 * #define STDOUT_FILENO 1
						 * #define STDERR_FILENO 2
						 *
						 */
						dup2(fd, STDIN_FILENO);  // 关闭标准输入
						dup2(fd, STDOUT_FILENO);  // 关闭标准输出
						dup2(fd, STDERR_FILENO);  // 关闭标准错误输出

						if (fd > 2)
						{
							close(fd);
						}
					}
				}

				/*
				 * mode_t umask(mode_t cmask)
				 *
				 * 027表示默认创建新文件权限为750
				 *
				 * 也就是rxwr-x---(所有者全部权限, 属组读写, 其它人无)
				 *
				 */
				umask(0027);
			}

		protected:
			static int Flock(const int32_t fd, const int32_t cmd, const int32_t type, const int32_t whence,
																					  const off_t start,
																					  const off_t len)
			{
				/*
			 * struct flock
			 * {
			 * 		short l_type;	// 加锁的类型: 只读锁(F_RDLCK), 读写锁(F_WRLCK), 或是解锁(F_UNLCK)
			 * 		off_t l_start;	// 加锁部分的开始位置 offset in bytes, relative to l_whence
			 * 		short l_whence;	// 加锁部分的开始位置 SEEK_SET, SEEK_CUR, or SEEK_END
			 * 		off_t l_len;	// 加锁的长度 length, in bytes; 0 means lock to EOF
			 * 		pid_t l_pid;	// 加锁进程的进程id returned with F_GETLK
			 * };
			 *
			 */
				struct flock lock = { };

				lock.l_len = len;
				lock.l_type = (short)type;
				lock.l_start = start;
				lock.l_whence = (short)whence;

				/*
				 * int fcntl(int fd, int cmd, struct flock *lock)
				 *
				 * 针对(文件)描述符提供控制
				 *
				 * fd 文件描述符
				 *
				 * cmd 操作命令
				 *
				 * lock 供命令使用的参数
				 *
				 */
				return fcntl(fd, cmd, &lock);
			}
		};
	}
}


#define TINY_PROCESS_FORK(...) tinyCore::system::Process::Fork(__VA_ARGS__)
#define TINY_PROCESS_IS_RUNNING(...) tinyCore::system::Process::IsRunning(__VA_ARGS__)


#endif // __TINY_CORE__SYSTEM__PROCESS__H__
