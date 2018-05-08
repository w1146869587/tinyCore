#ifndef __TINY_CORE__CRYPTO__MD5__H__
#define __TINY_CORE__CRYPTO__MD5__H__


/**
 *
 *  作者: hm
 *
 *  说明: md5编码
 *
 */


#include <tinyCore/crypto/hash.h>


namespace tinyCore
{
	namespace crypto
	{
		typedef EVPObject<boost::mpl::string<'m', 'd', '5'>> MD5;
	}
}


#endif // __TINY_CORE__CRYPTO__MD5__H__
