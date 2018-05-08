#
# 宏定义, 遍历指定目录获取源文件, TRAVERSE_SOURCE_FILE(arg, path)
#
MACRO(TRAVERSE_SOURCE_FILE)

	IF(NOT ${ARGC} LESS 3)

		MESSAGE(FATAL_ERROR "ARGC NUMBER ERROR")

	ENDIF()

	FILE(GLOB_RECURSE ${ARGV0}	"${ARGV1}/*.c"
								"${ARGV1}/*.cc"
								"${ARGV1}/*.cpp"
								"${ARGV1}/*.h"
								"${ARGV1}/*.hh"
								"${ARGV1}/*.hpp")

	FOREACH(file ${${ARGV0}})

		MESSAGE(STATUS "Found " ${PROGRAM_NAME} " Sources : " ${file})

	ENDFOREACH()

ENDMACRO(TRAVERSE_SOURCE_FILE)


#
# 宏定义, 遍历当前目录获取源文件, TRAVERSE_CURRENT_SOURCE_FILE(arg)
#
MACRO(TRAVERSE_CURRENT_SOURCE_FILE)

	IF(NOT ${ARGC} LESS 2)

		MESSAGE(FATAL_ERROR "ARGC NUMBER ERROR")

	ENDIF()

	FILE(GLOB_RECURSE ${ARGV0}	"*.c"
								"*.cc"
								"*.cpp"
								"*.h"
								"*.hh"
								"*.hpp")

	FOREACH(file ${${ARGV0}})

		MESSAGE(STATUS "Found " ${PROGRAM_NAME} " Sources : " ${file})

	ENDFOREACH()

ENDMACRO(TRAVERSE_CURRENT_SOURCE_FILE)
