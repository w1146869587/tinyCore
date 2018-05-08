#
# 宏定义, 遍历添加指定目录下级项目, TRAVERSE_CURRENT_SOURCE_FILE(path)
#
MACRO(TRAVERSE_ADD_SUBDIRECTORY)

	IF(NOT ${ARGC} LESS 2)

		MESSAGE(FATAL_ERROR "ARGC NUMBER ERROR")

	ENDIF()

	FILE(GLOB_RECURSE cmakeLists "${ARGV0}/CMakeLists.txt")

	FOREACH(cmake ${cmakeLists})

		STRING(LENGTH ${ARGV0}/ cmake_prefix_length)

		STRING(SUBSTRING ${cmake} ${cmake_prefix_length} -1 cmake_relative_path)

		STRING(FIND ${cmake_relative_path} "/" cmake_relative_pos)

		IF(NOT (${cmake_relative_pos} STREQUAL -1))

			STRING(FIND ${cmake_relative_path} "/CMakeLists.txt" cmake_suffix_pos)

			IF(${cmake_relative_pos} STREQUAL ${cmake_suffix_pos})

				STRING(SUBSTRING ${cmake_relative_path} 0 ${cmake_relative_pos} cmake_sub_directory)

				ADD_SUBDIRECTORY(${cmake_sub_directory})

			ENDIF()

		ENDIF()

	ENDFOREACH()

ENDMACRO(TRAVERSE_ADD_SUBDIRECTORY)


#
# 宏定义, 遍历添加当前目录下级项目, TRAVERSE_CURRENT_ADD_SUBDIRECTORY()
#
MACRO(TRAVERSE_CURRENT_ADD_SUBDIRECTORY)

	IF(NOT ${ARGC} LESS 1)

		MESSAGE(FATAL_ERROR "ARGC NUMBER ERROR")

	ENDIF()

	file(GLOB_RECURSE cmakeLists "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

	FOREACH(cmake ${cmakeLists})

		STRING(LENGTH ${CMAKE_CURRENT_LIST_DIR}/ cmake_prefix_length)

		STRING(SUBSTRING ${cmake} ${cmake_prefix_length} -1 cmake_relative_path)

		STRING(FIND ${cmake_relative_path} "/" cmake_relative_pos)

		IF(NOT (${cmake_relative_pos} STREQUAL -1))

			STRING(FIND ${cmake_relative_path} "/CMakeLists.txt" cmake_suffix_pos)

			IF(${cmake_relative_pos} STREQUAL ${cmake_suffix_pos})

				STRING(SUBSTRING ${cmake_relative_path} 0 ${cmake_relative_pos} cmake_sub_directory)

				ADD_SUBDIRECTORY(${cmake_sub_directory})

			ENDIF()

		ENDIF()

	ENDFOREACH()

ENDMACRO(TRAVERSE_CURRENT_ADD_SUBDIRECTORY)