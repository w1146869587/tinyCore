#ifndef __TINY_CORE__UTILITIES__OBJECT__H__
#define __TINY_CORE__UTILITIES__OBJECT__H__


/**
 *
 *  作者: hm
 *
 *  说明: 对象管理
 *
 */


#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace utilities
	{
		class TINY_API IObject
		{
		public:
			virtual ~IObject() = default;

			virtual void Release() = 0;
		};

		class TINY_API ObjectManager
		{
		public:
			static ObjectManager * GetInstance()
			{
				static ObjectManager instance;

				return &instance;
			}

			template <typename ValueT>
			ValueT * RegisterObject()
			{
				std::lock_guard<std::mutex> lock(_lock);

				if (IsRelease())
				{
					return nullptr;
				}

				ValueT * object = boost::factory<ValueT *>()();

				_objectList.push_back(object);

				return object;
			}

			void Release()
			{
				std::lock_guard<std::mutex> lock(_lock);

				if (IsRelease())
				{
					return;
				}

				for (auto &iter : _objectList)
				{
					iter->Release();
				}

				_objectList.clear();

				_isRelease = true;
			}

			bool IsRelease() const
			{
				return _isRelease;
			}

		private:
			ObjectManager() : _isRelease(false)
			{

			}

			~ObjectManager()
			{
				Release();
			}

		protected:
			bool _isRelease;

			std::mutex _lock;

			std::list<IObject *> _objectList{ };
		};

		template <typename TypeT>
		class TINY_API SingletonObject : public IObject
		{
		public:
			static TypeT * GetInstance()
			{
				if (!_object)
				{
					_object = ObjectManager::GetInstance()->RegisterObject<SingletonObject<TypeT>>();
				}

				return _object ? &(_object->_instance) : nullptr;
			}

			void Release() override
			{
				boost::checked_delete(this);

				_object = nullptr;
			}

		protected:
			TypeT _instance;

			static SingletonObject<TypeT> * _object;
		};

		template <typename TypeT>
		tinyCore::utilities::SingletonObject<TypeT> * SingletonObject<TypeT>::_object = nullptr;
	}
}


#define sObjectManager  		tinyCore::utilities::ObjectManager::GetInstance()
#define sSingletonObject(type)	tinyCore::utilities::SingletonObject<type>::GetInstance()


#endif // __TINY_CORE__UTILITIES__OBJECT__H__
