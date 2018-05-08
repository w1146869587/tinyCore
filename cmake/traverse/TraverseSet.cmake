#
# 宏定义, 包含多个设置, TRAVERSE_SET(project name1 name2 ...)
#
MACRO(TRAVERSE_SET)

	FOREACH(param ${ARGN})

		IF (NOT (${param} STREQUAL ${ARGV0}))

			SET(${ARGV0} "${${ARGV0}} ${param}")

		ENDIF()

	ENDFOREACH()

ENDMACRO(TRAVERSE_SET)
