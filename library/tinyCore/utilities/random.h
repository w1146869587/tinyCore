#ifndef __TINY_CORE__UTILITIES__RANDOM__H__
#define __TINY_CORE__UTILITIES__RANDOM__H__


/**
 *
 *  作者: hm
 *
 *  说明: 生成随机数
 *
 */


#include <random>

#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace utilities
	{
		class TINY_API Random
		{
		public:
			static double UniformDistribution(const double head, const double tail)
			{
				assert(head <= tail);

				std::random_device rd;

				std::mt19937 mt(rd());

				std::uniform_real_distribution<double> dis(head, tail);

				return dis(mt);
			}

			template<typename TypeT = int32_t>
			static TypeT UniformDistribution(const TypeT head = 0, const TypeT tail = INT_MAX)
			{
				assert(head <= tail);

				std::random_device rd;

				std::mt19937 mt(rd());

				std::uniform_int_distribution<TypeT> dis(head, tail);

				return dis(mt);
			}

			static void randBytes(Byte * buffer, const std::size_t length)
			{
				if (buffer)
				{
					std::random_device rd;

					std::uniform_int_distribution<Byte> dist;

					for (std::size_t i = 0; i != length; ++i)
					{
						buffer[i] = dist(rd);
					}
				}
			}

			template <std::size_t LENGTH>
			static void randBytes(std::array<Byte, LENGTH> & buffer)
			{
				std::random_device rd;

				std::uniform_int_distribution<Byte> dist;

				for (std::size_t i = 0; i != LENGTH; ++i)
				{
					buffer[i] = dist(rd);
				}
			}
		};
	}
}


#define TINY_RANDOM(type, head, tail) tinyCore::utilities::Random::UniformDistribution<type>(head, tail)

#define TINY_RANDOM_BYTE(value, ...) tinyCore::utilities::Random::randBytes(value, tail)


#endif // __TINY_CORE__UTILITIES__RANDOM__H__
