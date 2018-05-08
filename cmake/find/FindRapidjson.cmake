#
# 设置包名
#
SET(PROGRAM_NAME Rapidjson)


#
# 设置路径
#
SET(RAPIDJSON_SOFTWARE_PATH ${PROJECT_SOURCE_DIR}/library/rapidjson)


#
# 检查路径
#
IF (IS_DIRECTORY ${RAPIDJSON_SOFTWARE_PATH})

	#
	# 输出信息
	#
	MESSAGE(STATUS "Found " ${PROGRAM_NAME} " Directory : " ${RAPIDJSON_SOFTWARE_PATH})

	#
	# 设置路径
	#
	SET(RAPIDJSON_SOFTWARE_INCLUDE_PATH		${RAPIDJSON_SOFTWARE_PATH})

ELSE()

	#
	# 输出信息
	#
	MESSAGE(FATAL_ERROR "Not Found " ${PROGRAM_NAME} " Directory : " ${RAPIDJSON_SOFTWARE_PATH})

ENDIF()