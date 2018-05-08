#ifndef __TINY_CORE__POOL__THREAD_POOL__H__
#define __TINY_CORE__POOL__THREAD_POOL__H__


/**
 *
 *  作者: hm
 *
 *  说明: 线程池
 *
 */


#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace pool
	{
		class ThreadPool
		{
			using Task = std::function<void()>;

		public:
			ThreadPool()
			{
				_isStop.store(false);
			}

			~ThreadPool()
			{
				_isStop.store(true);

				_condition.notify_all();

				for (std::thread &thread : _pool)
				{
					if (thread.joinable())
					{
						thread.join();
					}
				}
			}

			std::size_t TaskCount() const
			{
				return _tasks.size();
			}

			std::size_t ThreadCount() const
			{
				return _pool.size();
			}

			std::size_t FreeCount() const
			{
				return _freeSize;
			}

			bool IsWork()
			{
				return !_tasks.empty() || (_pool.size() != _freeSize);
			}

			void Launch(const std::size_t size)
			{
				_freeSize.store(size);

				for (std::size_t i = 0; i < size; ++i)
				{
					_pool.emplace_back
					(
						[this]
						{
							// 工作线程函数
							while (!this->_isStop)
							{
								std::function<void()> task;

								// 获取一个待执行的 task
								{
									// unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()
									std::unique_lock<std::mutex> lock(this->_lock);

									// wait 直到有 task
									this->_condition.wait
									(
										lock,

										[this]
										{
											return this->_isStop.load() || !this->_tasks.empty();
										}
									);

									if (this->_isStop && this->_tasks.empty())
									{
										return false;
									}

									task = std::move(this->_tasks.front()); // 取一个 task

									this->_tasks.pop();
								}

								--_freeSize;

								task();

								++_freeSize;
							}

							return true;
						}
					);
				}
			}

			// 提交一个任务
			// 调用.get()获取返回值会等待任务执行完,获取返回值
			// 有两种方法可以实现调用类成员
			// 一种是使用   bind： .commit(std::bind(&Dog::sayHello, &dog));
			// 一种是使用 mem_fn： .commit(std::mem_fn(&Dog::sayHello), &dog)
			template<class Func, class... Args>
			auto Commit(Func && func, Args &&... args) ->std::future<decltype(func(args...))>
			{
				TINY_THROW_EXCEPTION_IF(_isStop.load(), debug::ThreadError, "commit on ThreadPool is stopped")

				using RetType = decltype(func(args...));

				auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<Func>(func),
																					  std::forward<Args>(args)...));

				std::future<RetType> res = task->get_future();

				{
					std::unique_lock<std::mutex> lock(_lock);

					_tasks.emplace
					(
						[task]()
						{
							(*task)();
						}
					);
				}

				_condition.notify_one();  // 唤醒一个线程执行

				return res;
			};

		protected:
			std::mutex _lock{ };

			std::queue<Task> _tasks{ };

			std::atomic<bool> _isStop{ };
			std::atomic<std::size_t> _freeSize{ };

			std::vector<std::thread> _pool{ };

			std::condition_variable _condition{ };
		};
	}
}


#endif // __TINY_CORE__POOL__THREAD_POOL__H__
