#ifndef __TINY_CORE__ID__GUID__H__
#define __TINY_CORE__ID__GUID__H__


/**
 *
 *  作者: hm
 *
 *  说明: 获取guid
 *
 */


#include <tinyCore/utilities/time.h>


namespace tinyCore
{
	namespace id
	{
		class GUID
		{
			union ONLY_ID
			{
				struct
				{
					uint32_t count;
					uint32_t times;
				}key;

				uint64_t value;
			};

		public:
			static uint64_t Create()
			{
				return GetInstance().CreateID();
			}

		protected:
			static GUID & GetInstance()
			{
				static GUID instance;

				return instance;
			}

			uint64_t CreateID()
			{
				auto timeStamp = static_cast<uint32_t>(TINY_TIME_MILLISECONDS());

				if (timeStamp != _id.key.times)
				{
					_id.key.count = 0;
					_id.key.times = timeStamp;
				}
				else
				{
					++_id.key.count;
				}

				return _id.value;
			}

		protected:
			static ONLY_ID _id;
		};
	}
}


#define TINY_ID_GUID() tinyCore::id::GUID::Create()


#endif // __TINY_CORE__ID__GUID__H__
