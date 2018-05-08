#ifndef __TINY_CORE__CRYPTO__SHA__H__
#define __TINY_CORE__CRYPTO__SHA__H__


/**
 *
 *  作者: hm
 *
 *  说明: sha编码
 *
 */


#include <tinyCore/crypto/hash.h>


namespace tinyCore
{
	namespace crypto
	{
		typedef EVPObject<boost::mpl::string<'s', 'h', 'a', '1'>> SHA1;

		#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER >= 0x00908000)

		typedef EVPObject<boost::mpl::string<'s', 'h', 'a', '2', '2', '4'>> SHA224;
		typedef EVPObject<boost::mpl::string<'s', 'h', 'a', '2', '5', '6'>> SHA256;
		typedef EVPObject<boost::mpl::string<'s', 'h', 'a', '3', '8', '4'>> SHA384;
		typedef EVPObject<boost::mpl::string<'s', 'h', 'a', '5', '1', '2'>> SHA512;

		#endif
	}
}





#endif // __TINY_CORE__CRYPTO__SHA__H__
