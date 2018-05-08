#ifndef __TINY_CORE__COMMON__MACRO__H__
#define __TINY_CORE__COMMON__MACRO__H__


/**
 *
 * 转换成字符串
 *
 */
#define TINY_CONVERT_STRING(value) #value
#define TINY_CONVERT_VALUE_STRING(value) TINY_CONVERT_STRING(value)


/**
 *
 * 平台类型
 *
 */
#define TINY_PLATFORM_UNIX    1
#define TINY_PLATFORM_APPLE   2
#define TINY_PLATFORM_INTEL   3
#define TINY_PLATFORM_WINDOWS 4


/**
 *
 * 编译器类型
 *
 */
#define TINY_COMPILER_GNU       0
#define TINY_COMPILER_INTEL     1
#define TINY_COMPILER_BORLAND   2
#define TINY_COMPILER_MICROSOFT 3


/**
 *
 * 圆周率
 *
 */
#define TINY_PI					3.1415926535898f


/**
 *
 * 时间
 *
 */
#define TINY_SECOND				(1)
#define TINY_MINUTE				(60 * TINY_SECOND)
#define TINY_HOUR				(60 * TINY_MINUTE)
#define TINY_DAY				(24 * TINY_HOUR)
#define TINY_WEEK				(7 * TINY_DAY)
#define TINY_YEAR				(12 * TINY_WEEK)


/**
 *
 * 数据大小
 *
 */
#define TINY_BYTE				(1)
#define TINY_KB					(1024 * TINY_BYTE)
#define TINY_MB					(1024 * TINY_KB)
#define TINY_GB					(1024 * TINY_MB)
#define TINY_TB					(1024 * TINY_GB)


/**
 *
 * 数据包首部长度
 *
 */
#define TINY_IP_HEADER_LEN		20
#define TINY_ETH_HEADER_LEN		14
#define TINY_TCP_HEADER_LEN		20
#define TINY_UDP_HEADER_LEN		8


/**
 *
 * 位操作
 *
 */
#define TINY_SET_MASK(lhs, rhs)  lhs |= (rhs)
#define TINY_CLR_MASK(lhs, rhs)  lhs &= (~(rhs))
#define TINY_TST_MASK(lhs, rhs)  (((lhs) & (rhs)) == (rhs))


#endif // __TINY_CORE__COMMON__MACRO__H__
