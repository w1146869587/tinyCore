#ifndef __TINY_CORE__ID__UUID__H__
#define __TINY_CORE__ID__UUID__H__


/**
 *
 *  作者: hm
 *
 *  说明: 获取uuid
 *
 */


#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <tinyCore/utilities/string.h>


namespace tinyCore
{
	namespace id
	{
		class UUID
		{
		public:
			static uint64_t Create()
			{
				return boost::uuids::hash_value(GetInstance().GetRandUUID());
			}

			static uint64_t Create(const std::string & key)
			{
				return boost::uuids::hash_value(GetInstance().GetNameUUID(key));
			}

			static uint64_t Create(const std::string & key, const std::string & groupKey)
			{
				return boost::uuids::hash_value(GetInstance().GetGroupUUID(key, groupKey));
			}

			static std::string CreateStr()
			{
				return TINY_STR_TO_HEX_STRING(std::string((const char *)GetInstance().GetRandUUID().data, 16));
			}

			static std::string CreateStr(const std::string & key)
			{
				return TINY_STR_TO_HEX_STRING(std::string((const char *)GetInstance().GetNameUUID(key).data, 16));
			}

			static std::string CreateStr(const std::string & key, const std::string & groupKey)
			{
				return TINY_STR_TO_HEX_STRING(std::string((const char *)GetInstance().GetGroupUUID(key, groupKey).data, 16));
			}

		private:
			static UUID & GetInstance()
			{
				static UUID instance;

				return instance;
			}

			boost::uuids::uuid GetRandUUID()
			{
				boost::uuids::random_generator generator;

				return generator();
			}

			boost::uuids::uuid GetNameUUID(const std::string & key)
			{
				return boost::uuids::string_generator()(key);
			}

			boost::uuids::uuid GetGroupUUID(const std::string & key, const std::string & groupKey)
			{
				boost::uuids::uuid groupGenerator = boost::uuids::string_generator()(groupKey);

				return boost::uuids::name_generator(groupGenerator)(key);
			}
		};
	}
}


#define TINY_ID_UUID(...) tinyCore::id::UUID::Create(__VA_ARGS__)
#define TINY_ID_UUID_STR(...) tinyCore::id::UUID::CreateStr(__VA_ARGS__)



#endif // __TINY_CORE__ID__UUID__H__
