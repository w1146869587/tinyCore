#
# 添加cmake自带的模块
#
INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCXXCompilerFlag)


#
# 检测C编译的特性
#
#CHECK_C_COMPILER_FLAG("-std=c89" C_COMPILER_SUPPORTS_C89)
#CHECK_C_COMPILER_FLAG("-std=c99" C_COMPILER_SUPPORTS_C99)
#CHECK_C_COMPILER_FLAG("-std=c11" C_COMPILER_SUPPORTS_C11)
#
#IF(C_COMPILER_SUPPORTS_C11)
#
#	SET(CMAKE_C_FLAGS "-std=c11")
#
#ELSEIF(C_COMPILER_SUPPORTS_C99)
#
#	SET(CMAKE_C_FLAGS "-std=c99")
#
#ELSEIF(C_COMPILER_SUPPORTS_C89)
#
#	SET(CMAKE_C_FLAGS "-std=c89")
#
#ELSE()
#
#	MESSAGE(STATUS "The C compiler ${CMAKE_C_COMPILER} does not support more than C89 features.")
#
#ENDIF()


#
# 检测C++编译器支持的特性
#
CHECK_CXX_COMPILER_FLAG("-std=c++0x" CXX_COMPILER_SUPPORTS_CXX0X)
CHECK_CXX_COMPILER_FLAG("-std=c++11" CXX_COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++14" CXX_COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++17" CXX_COMPILER_SUPPORTS_CXX17)

IF(CXX_COMPILER_SUPPORTS_CXX17)

	SET(CMAKE_CXX_FLAGS "-std=c++17")

ELSEIF(CXX_COMPILER_SUPPORTS_CXX14)

	SET(CMAKE_CXX_FLAGS "-std=c++14")

ELSEIF(CXX_COMPILER_SUPPORTS_CXX11)

	SET(CMAKE_CXX_FLAGS "-std=c++11")

ELSEIF(CXX_COMPILER_SUPPORTS_CXX0X)

	SET(CMAKE_CXX_FLAGS "-std=c++0x")

ELSE()

	MESSAGE(STATUS "The CXX compiler ${CMAKE_CXX_COMPILER} does not support more than C++11 features.")

ENDIF()


#
# 设置通用编译选项
#
# -Wshadow
# -Winline
#
# -Werror
# -Wfatal-errors
#
# -pg // gproftools 冲突
#
TRAVERSE_SET(COMPILER_FLAGS	-g
							-Os
							-lz
							-ldl
							-fPIC
							-pthread
							-rdynamic
							-Wall
							-Wstrict-aliasing
							-Wunreachable-code
							-Wreturn-local-addr
							-Wdeprecated-declarations)


#
# 设置clang asan选项
#
TRAVERSE_SET(COMPILER_CLANG_ASAN_FLAG	-fsanitize=address
										-fno-omit-frame-pointer)


#
# 设置gcc asan选项
#
# -fsanitize=kernel-address
# -fsanitize=address-use-after-scope
TRAVERSE_SET(COMPILER_GCC_ASAN_FLAG	-static-libasan
									-static-libubsan
									-fsanitize=leak
									-fsanitize=vptr
									-fsanitize=null
									-fsanitize=shift
									-fsanitize=bounds
									-fsanitize=return
									-fsanitize=address
									-fsanitize=alignment
									-fsanitize=undefined
									-fsanitize=vla-bound
									-fsanitize=object-size
									-fsanitize=unreachable
									-fsanitize=bounds-strict
									-fsanitize=float-cast-overflow
									-fsanitize=float-divide-by-zero
									-fsanitize=integer-divide-by-zero
									-fsanitize=signed-integer-overflow
									-fstack-protector
									-fno-strict-aliasing
							 		-fno-omit-frame-pointer
									-fno-delete-null-pointer-checks)


#
# 设置C/C++编译选项
#
SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${COMPILER_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS}")


#
# 根据编译器来选择asan编译选项
#
IF(CMAKE_CXX_COMPILER_ID MATCHES GNU)

	IF(${CMAKE_BUILD_TYPE} MATCHES Debug)

		SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${COMPILER_GCC_ASAN_FLAG}")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_GCC_ASAN_FLAG}")

	ENDIF()

ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES Clang)

	IF(${CMAKE_BUILD_TYPE} MATCHES Debug)

		SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${COMPILER_CLANG_ASAN_FLAG}")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_CLANG_ASAN_FLAG}")

	ENDIF()

ENDIF()
