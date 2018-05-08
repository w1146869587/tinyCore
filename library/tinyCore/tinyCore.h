#ifndef __TINY_CORE__TINY_CORE__H__
#define __TINY_CORE__TINY_CORE__H__


// common
#include <tinyCore/common/macro.h>
#include <tinyCore/common/common.h>
#include <tinyCore/common/version.h>
#include <tinyCore/common/platform.h>
#include <tinyCore/common/compiler.h>

// compress
#include <tinyCore/compress/zlib.h>
#include <tinyCore/compress/gzip.h>

// configuration
#include <tinyCore/configuration/option.h>
#include <tinyCore/configuration/config.h>

// container
#include <tinyCore/container/queue.h>
#include <tinyCore/container/message.h>
#include <tinyCore/container/memcached.h>

// crypto
#include <tinyCore/crypto/url.h>
#include <tinyCore/crypto/md5.h>
#include <tinyCore/crypto/sha.h>
#include <tinyCore/crypto/hash.h>
#include <tinyCore/crypto/base64.h>
#include <tinyCore/crypto/license.h>

// debug
#include <tinyCore/debug/trace.h>
#include <tinyCore/debug/exception.h>
#include <tinyCore/debug/backtrace.h>

// http
#include <tinyCore/http/httpClient.h>

// id
#include <tinyCore/id/pid.h>
#include <tinyCore/id/guid.h>
#include <tinyCore/id/uuid.h>
#include <tinyCore/id/threadID.h>
#include <tinyCore/id/snowflake.h>

// lock
#include <tinyCore/lock/mutex.h>
#include <tinyCore/lock/atomic.h>

// log
#include <tinyCore/log/file.h>
#include <tinyCore/log/sink.h>
#include <tinyCore/log/syslog.h>
#include <tinyCore/log/detail.h>
#include <tinyCore/log/logger.h>
#include <tinyCore/log/registry.h>
#include <tinyCore/log/formatter.h>

// pool
#include <tinyCore/pool/appPool.h>
#include <tinyCore/pool/threadPool.h>
#include <tinyCore/pool/callBackPool.h>

//sql
#include <tinyCore/sql/sqlite.h>

// system
#include <tinyCore/system/serial.h>
#include <tinyCore/system/signal.h>
#include <tinyCore/system/process.h>
#include <tinyCore/system/fileSystem.h>
#include <tinyCore/system/networkCard.h>

// thread
#include <tinyCore/thread/appThread.h>

// utilities
#include <tinyCore/utilities/ip.h>
#include <tinyCore/utilities/time.h>
#include <tinyCore/utilities/object.h>
#include <tinyCore/utilities/random.h>
#include <tinyCore/utilities/string.h>
#include <tinyCore/utilities/digital.h>


#endif // __TINY_CORE__TINY_CORE__H__
