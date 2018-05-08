#ifndef __TINY_CORE__CRYPTO__HASH__H__
#define __TINY_CORE__CRYPTO__HASH__H__


/**
 *
 *  作者: hm
 *
 *  说明: hash编码
 *
 */


#include <openssl/err.h>
#include <openssl/evp.h>

#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace crypto
	{
		#if (OPENSSL_VERSION_NUMBER < 0x10100000L) || defined(LIBRESSL_VERSION_NUMBER)
		#
		#define EVP_MD_CTX_new EVP_MD_CTX_create
		#define EVP_MD_CTX_free EVP_MD_CTX_destroy
		#
		#endif

		class TINY_API EVPContext
		{
		public:
			explicit EVPContext(const char * name)
			{
				if (name)
				{
					const EVP_MD * type = EVP_get_digestbyname(name);

					_ctx = EVP_MD_CTX_new();

					EVP_DigestInit(_ctx, type);
				}
			}

			~EVPContext()
			{
				if (_ctx)
				{
					EVP_MD_CTX_free(_ctx);

					_ctx = nullptr;
				}
			}

			const EVP_MD_CTX * GetContext() const
			{
				return _ctx;
			}

		protected:
			EVP_MD_CTX * _ctx{ nullptr };
		};

		template <typename HashNameType>
		class EVPObject
		{
		public:
			EVPObject()
			{
				_ctx = EVP_MD_CTX_new();

				EVP_MD_CTX_copy(_ctx, Initializer().GetContext());
			}

			explicit EVPObject(const char * data) : EVPObject()
			{
				if (data)
				{
					Update(data);
				}
			}

			explicit EVPObject(const std::string & data) : EVPObject()
			{
				Update(data);
			}

			explicit EVPObject(const std::vector<Byte> & data) : EVPObject()
			{
				Update(data);
			}

			explicit EVPObject(const Byte * data, std::size_t size) : EVPObject()
			{
				if (data)
				{
					Update(data, size);
				}
			}

			EVPObject(EVPObject && rhs) noexcept
			{
				_ctx = rhs._ctx;

				rhs._ctx = nullptr;
			}

			EVPObject(const EVPObject & lhs)
			{
				_ctx = EVP_MD_CTX_new();

				EVP_MD_CTX_copy(_ctx, lhs._ctx);
			}

			~EVPObject()
			{
				Reset();
			}

			void Update(const char * data)
			{
				TINY_ASSERT(data, "data is nullptr");

				Update((const Byte *)data, strlen(data));
			}

			void Update(const std::string & data)
			{
				Update((const Byte *)data.data(), data.size());
			}

			void Update(const std::vector<Byte> & data)
			{
				Update(data.data(), data.size());
			}

			void Update(const Byte * data, const std::size_t size)
			{
				TINY_ASSERT(data, "data is nullptr");

				EVP_DigestUpdate(_ctx, data, size);
			}

			int32_t GetContextSize() const
			{
				return EVP_MD_CTX_size(_ctx);
			}

			int32_t GetContextBlockSize() const
			{
				return EVP_MD_CTX_block_size(_ctx);
			}

			std::string Hex() const
			{
				EVPObject temp(*this);

				unsigned char digest[EVP_MAX_MD_SIZE];

				auto digestSize = temp.GetContextSize();

				EVP_DigestFinal(temp._ctx, digest, nullptr);

				std::string result;

				char c;

				for(int32_t i = 0; i < digestSize; i++)
				{
					c = (char)((digest[i] >> 4) & 0xf);
					c = (char)((c > 9) ? c +'a'-10 : c + '0');

					result.push_back(c);

					c = (char)(digest[i] & 0xf);
					c = (char)((c > 9) ? c +'a'-10 : c + '0');

					result.push_back(c);
				}

				return result;
			}

			std::string Digest() const
			{
				EVPObject temp(*this);

				unsigned char digest[EVP_MAX_MD_SIZE];

				auto digestSize = temp.GetContextSize();

				EVP_DigestFinal(temp._ctx, digest, nullptr);

				std::string result;

				result.assign((char *)digest, (uint32_t)digestSize);

				return result;
			}

			EVPObject & operator=(const EVPObject & rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}

				Reset();

				_ctx = EVP_MD_CTX_new();

				EVP_MD_CTX_copy(_ctx, rhs._ctx);

				return *this;
			}

			EVPObject & operator=(EVPObject && rhs) noexcept
			{
				Reset();

				_ctx = rhs._ctx;

				rhs._ctx = nullptr;

				return *this;
			}

		protected:
			void Reset()
			{
				if (_ctx)
				{
					EVP_MD_CTX_free(_ctx);

					_ctx = nullptr;
				}
			}

			static const EVPContext & Initializer()
			{
				static const EVPContext context(boost::mpl::c_str<HashNameType>::value);

				return context;
			}

		protected:
			EVP_MD_CTX * _ctx{ nullptr };
		};
	}
}


#endif // __TINY_CORE__CRYPTO__HASH__H__
