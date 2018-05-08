#ifndef __TINY_CORE__POOL__CALL_BACK_POOL__H__
#define __TINY_CORE__POOL__CALL_BACK_POOL__H__


/**
 *
 *  作者: hm
 *
 *  说明: 回调池
 *
 */


#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace pool
	{
		template<typename FunctionT>
		struct CallBack
		{
		public:
			CallBack()
			{
				memset(this, 0, sizeof(*this));
			}

			CallBack(FunctionT pFunction, const char * pInfo)
			{
				info.assign(pInfo);

				function = pFunction;
			}

			CallBack<FunctionT> & operator = (const CallBack<FunctionT> & target)
			{
				memcpy(this, &target, sizeof(target));

				return *this;
			}

			bool operator == (const CallBack<FunctionT> & target)
			{
				return (this->function == target.function);
			}

			bool operator < (const CallBack<FunctionT> & target)
			{
				return (this->function < target.function);
			}

		public:
			std::string info{ };

			FunctionT function{ };
		};

		template<typename TypeT, typename FunctionT, typename... Args>
		class CallBackPool
		{
			typedef CallBack<FunctionT> CALLBACK;
			typedef std::list<CALLBACK> CALLBACK_POOL;
			typedef std::unordered_map<TypeT, CALLBACK_POOL, HashFunc<TypeT>, CompareFunc<TypeT>> CALLBACK_POOL_MAP;

		public:
			bool Register(const TypeT id, const FunctionT pFunction, const char * pInfo = "no info")
			{
				auto findPtr = _container.find(id);

				if (findPtr == _container.end())
				{
					_container[id] = CALLBACK_POOL();

					findPtr = _container.find(id);
				}

				findPtr->second.push_back(CALLBACK(pFunction, pInfo));

				return true;
			}

			bool UnRegister(const TypeT id, const FunctionT pFunction)
			{
				auto findPtr = _container.find(id);

				if (findPtr == _container.end())
				{
					_container[id] = CALLBACK_POOL();

					findPtr = _container.find(id);
				}

				auto iter = std::find(findPtr->second.begin(), findPtr->second.end(), CALLBACK(pFunction, nullptr));

				if (iter == findPtr->second.end())
				{
					return false;
				}

				findPtr->second.erase(iter);

				return true;
			}

			void UnRegister(const TypeT id)
			{
				auto findPtr = _container.find(id);

				if (findPtr != _container.end())
				{
					findPtr->second.clear();
				}
			}

			void Call(const TypeT id, Args... args)
			{
				auto findPtr = _container.find(id);

				if (findPtr == _container.end())
				{
					return;
				}

				for (auto &iter : findPtr->second)
				{
					iter.function(args...);
				}
			}

		private:
			CALLBACK_POOL_MAP _container;
		};
	}
}


#endif // __TINY_CORE__POOL__CALL_BACK_POOL__H__
