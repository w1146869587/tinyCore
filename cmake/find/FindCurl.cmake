#
# 设置包名
#
SET(PROGRAM_NAME Curl)


#
# 设置路径
#
SET(CURL_SOFTWARE_PATH /home/software/curl/curl-7.55.0)


#
# 检查路径
#
IF (IS_DIRECTORY ${CURL_SOFTWARE_PATH})

	#
	# 输出信息
	#
	MESSAGE(STATUS "Found " ${PROGRAM_NAME} " Directory : " ${CURL_SOFTWARE_PATH})

	#
	# 设置路径
	#
	SET(CURL_SOFTWARE_LIBRARY_PATH		${CURL_SOFTWARE_PATH}/lib)
	SET(CURL_SOFTWARE_INCLUDE_PATH		${CURL_SOFTWARE_PATH}/include)

ELSE()

	#
	# 输出信息
	#
	MESSAGE(FATAL_ERROR "Not Found " ${PROGRAM_NAME} " Directory : " ${CURL_SOFTWARE_PATH})

ENDIF()
