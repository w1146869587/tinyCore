#
# 设置包名
#
SET(PROGRAM_NAME Boost)


#
# 设置路径
#
SET(BOOST_SOFTWARE_PATH /home/software/boost/boost-1.65.0)


#
# 检查路径
#
IF (IS_DIRECTORY ${BOOST_SOFTWARE_PATH})

	#
	# 输出信息
	#
	MESSAGE(STATUS "Found " ${PROGRAM_NAME} " Directory : " ${BOOST_SOFTWARE_PATH})

	#
	# 设置路径
	#
	SET(BOOST_SOFTWARE_LIBRARY_PATH		${BOOST_SOFTWARE_PATH}/lib)
	SET(BOOST_SOFTWARE_INCLUDE_PATH		${BOOST_SOFTWARE_PATH}/include)

ELSE()

	#
	# 输出信息
	#
	MESSAGE(FATAL_ERROR "Not Found " ${PROGRAM_NAME} " Directory : " ${BOOST_SOFTWARE_PATH})

ENDIF()
