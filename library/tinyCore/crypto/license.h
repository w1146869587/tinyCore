#ifndef __TINY_CORE__CRYPTO__LICENSE__H__
#define __TINY_CORE__CRYPTO__LICENSE__H__


/**
 *
 *  作者: hm
 *
 *  说明: 授权处理
 *
 */


#include <openssl/md5.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

#include <tinyCore/debug/trace.h>
#include <tinyCore/utilities/time.h>
#include <tinyCore/utilities/random.h>


namespace tinyCore
{
	namespace crypto
	{
		class License
		{
		public:
			static std::string Authorized(const char * input, const char * pem, const std::string & time)
			{
				TINY_ASSERT(pem, "pem is nullptr");
				TINY_ASSERT(input, "input is nullptr");

				uint32_t signLen = 0;

				std::string licenseCode;

				auto sign = Sign(pem, input, &signLen);

				if (time.empty())
				{
					licenseCode = TINY_STR_TO_HEX_STRING(sign, signLen);
				}
				else
				{
					std::size_t index = TINY_RANDOM(std::size_t, 0, 128) * 2;

					std::string timeHex = TINY_STR_TO_HEX_STRING(time);
					std::string indexHex = TINY_DIGITAL_TO_HEX_STR(index);

					licenseCode = indexHex + TINY_STR_TO_HEX_STRING(sign, signLen).insert(index, timeHex);
				}

				free(sign);

				return licenseCode;
			}

			static bool Validation(const char * input, const char * pem, const std::string & license)
			{
				TINY_ASSERT(pem, "pem is nullptr");
				TINY_ASSERT(input, "input is nullptr");

				std::string licenseCode;
				std::string licenseTime;

				if (license.size() == 256)
				{
					licenseCode = TINY_HEX_STR_TO_STRING(license);
				}
				else if (license.size() == 296)
				{
					auto index = TINY_HEX_STR_TO_DIGITAL(std::size_t, license.substr(0, 2));

					licenseTime = TINY_HEX_STR_TO_STRING(license.substr(index + 2, 38));
					licenseCode = TINY_HEX_STR_TO_STRING(license.substr(2, index) + license.substr(index + 40));
				}
				else
				{
					TINY_THROW_EXCEPTION(debug::ValueError, "license error")
				}

				if (Verify(input, pem, licenseCode.c_str()))
				{
					if (licenseTime.empty())  // 永久授权
					{
						return true;
					}
					else
					{
						return TINY_TIME_SECONDS() > TINY_TIME_FROM_TIME_STRING(licenseTime);
					}
				}

				return false;
			}

		protected:
			static bool Verify(const char * input, const char * pem, const char * license)
			{
				TINY_ASSERT(pem, "pem is nullptr");
				TINY_ASSERT(input, "input is nullptr");
				TINY_ASSERT(license, "license is nullptr");

				FILE * file = fopen(pem, "r");

				TINY_THROW_EXCEPTION_IF(file == nullptr, debug::FileError, TINY_STR_FORMAT("{}({})", strerror(errno), pem))

				RSA * rsa = PEM_read_RSA_PUBKEY(file, nullptr, nullptr, nullptr);

				TINY_THROW_EXCEPTION_IF(rsa == nullptr, debug::FileError, "RSA read public key error")

				int status = RSA_verify(NID_md5, (uint8_t *)input, (uint32_t)strlen(input),
												 (uint8_t *)license, (uint32_t)strlen(license),
												 rsa);

				RSA_free(rsa);

				return (status == 1);
			}

			static uint8_t * Sign(const char * input, const char * pem, uint32_t * signLen)
			{
				TINY_ASSERT(pem, "pem is nullptr");
				TINY_ASSERT(input, "input is nullptr");
				TINY_ASSERT(signLen, "signLen is nullptr");

				FILE * file = fopen(pem, "r");

				TINY_THROW_EXCEPTION_IF(file == nullptr, debug::FileError, TINY_STR_FORMAT("{}({})", strerror(errno), pem))

				RSA * rsa = PEM_read_RSAPrivateKey(file, nullptr, nullptr, nullptr);

				TINY_THROW_EXCEPTION_IF(rsa == nullptr, debug::FileError, "RSA read private key error")

				auto * sign = (uint8_t *)calloc((size_t)RSA_size(rsa), sizeof(uint8_t));

				TINY_THROW_EXCEPTION_IF(sign == nullptr, debug::MemoryError, "Unable to allocate memory for sign string")

				RSA_sign(NID_md5, (uint8_t *)input, (uint32_t)strlen(input), sign, signLen, rsa);

				RSA_free(rsa);

				return sign;
			}
		};
	}
}


#define TINY_LICENSE_AUTHORIZED(input, pem, time) tinyCore::crypto::License::Authorized(input, pem, time)
#define TINY_LICENSE_VALIDATION(input, pem, license) tinyCore::crypto::License::Validation(input, pem, license)


#endif // __TINY_CORE__CRYPTO__LICENSE__H__
