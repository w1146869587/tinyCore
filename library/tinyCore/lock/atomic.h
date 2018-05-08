#ifndef __TINY_CORE__LOCK__ATOMIC__H__
#define __TINY_CORE__LOCK__ATOMIC__H__


/**
 *
 *  作者: hm
 *
 *  说明: atomic
 *
 */


#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace lock
	{
//		using SystemAtomic = std::atomic;

		template <typename TypeT>
		class NullAtomic
		{
		public:
			NullAtomic() = default;

			explicit NullAtomic(TypeT val) : value(val)
			{

			}

			void store(TypeT val)
			{
				value = val;
			}

			TypeT load(std::memory_order) const
			{
				return value;
			}

		public:
			TypeT value{ };
		};
	}
}


#endif // __TINY_CORE__LOCK__ATOMIC__H__
