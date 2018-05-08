#ifndef __TINY_CORE__ID__SNOWFLAKE__H__
#define __TINY_CORE__ID__SNOWFLAKE__H__


/**
 *
 *  作者: hm
 *
 *  说明: twitter生成唯一全局id
 *
 */


#include <tinyCore/debug/trace.h>
#include <tinyCore/utilities/time.h>


namespace tinyCore
{
	namespace id
	{
		// 无效位                        时间戳                            数据标识  机器ID      序列号
		// 0 - 0000000000 0000000000 0000000000 0000000000 0000000000 0 - 00000 - 00000 - 000000000000
		class Snowflake
		{
		public:
			static uint64_t Create(const int32_t dataCenterID = 0, const int32_t workerID = 0)
			{
				return GetInstance().CreateID(dataCenterID, workerID);
			}

		protected:
			Snowflake()
			{
				_twepoch = 1514736000000;  // 2018-01-01

				_sequenceBits = 12;
				_workerIDBits = 5;
				_dataCenterIDBits = 5;

				_workerIDMax = ~(-1 * (1 << _workerIDBits));
				_sequenceMask = ~(-1 * (1 << _sequenceBits));
				_dataCenterIDMax = ~(-1 * (1 << _dataCenterIDBits));

				_workerIDShift = _sequenceBits;
				_dataCenterIDShift = _sequenceBits + _workerIDBits;
				_timeStampLeftShift = _sequenceBits + _workerIDBits + _dataCenterIDBits;
			}

			~Snowflake() = default;

			static Snowflake & GetInstance()
			{
				static Snowflake instance;

				return instance;
			}

			uint64_t CreateID(const int32_t dataCenterID, const int32_t workerID)
			{
				if (workerID < 0 || workerID > _workerIDMax)
				{
					TINY_THROW_EXCEPTION(debug::Exception, TINY_STR_FORMAT("workerID can't be greater than {} or less than 0", _workerIDMax))
				}

				if (dataCenterID < 0 || dataCenterID > _dataCenterIDMax)
				{
					TINY_THROW_EXCEPTION(debug::Exception, TINY_STR_FORMAT("dataCenterID can't be greater than {} or less than 0", _dataCenterIDMax))
				}

				if (workerID > 0)
				{
					_workerID = workerID;
				}

				if (dataCenterID > 0)
				{
					_dataCenterID = dataCenterID;
				}

				return NextID();
			}

			uint64_t NextID()
			{
				auto timeStamp = TINY_TIME_MILLISECONDS();

				if (timeStamp < _lastTimeStamp)
				{
					TINY_THROW_EXCEPTION(debug::Exception, "Clock moved backwards, Refusing to generate id")
				}

				if (_lastTimeStamp == timeStamp)
				{
					_sequence = (_sequence + 1) & _sequenceMask;

					if (_sequence == 0)  // 毫秒内序列溢出
					{
						timeStamp = TilNextMillis(_lastTimeStamp);
					}
				}
				else
				{
					_sequence = 0;
				}

				_lastTimeStamp = timeStamp;

				return (uint64_t)((timeStamp - _twepoch) << _timeStampLeftShift) |
					   (_dataCenterID << _dataCenterIDShift) |
					   (_workerID << _workerIDShift) |
					   _sequence;
			}

			time_t TilNextMillis(time_t lastTimestamp)
			{
				auto timeStamp = TINY_TIME_MILLISECONDS();

				while (timeStamp <= _lastTimeStamp)
				{
					timeStamp = TINY_TIME_MILLISECONDS();
				}

				return timeStamp;
			}

		protected:
			time_t _twepoch{ 0 };
			time_t _lastTimeStamp{ 0 };

			int32_t _sequence{ 0 };
			int32_t _workerID{ 0 };
			int32_t _dataCenterID{ 0 };

			int32_t _sequenceBits{ 0 };
			int32_t _workerIDBits{ 0 };
			int32_t _dataCenterIDBits{ 0 };

			int32_t _workerIDMax{ 0 };
			int32_t _dataCenterIDMax{ 0 };

			int32_t _workerIDShift{ 0 };
			int32_t _dataCenterIDShift{ 0 };
			int32_t _timeStampLeftShift{ 0 };

			int32_t _sequenceMask{ 0 };
		};
	}
}


#define TINY_ID_SNOWFLAKE(...) tinyCore::id::Snowflake::Create(__VA_ARGS__)


#endif // __TINY_CORE__ID__SNOWFLAKE__H__
