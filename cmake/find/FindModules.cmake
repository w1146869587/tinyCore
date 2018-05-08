#
# 设置包名
#
SET(PROGRAM_NAME Modules)


#
# 设置路径
#
SET(MODULES_SOFTWARE_PATH /lib/modules/${CMAKE_SYSTEM_VERSION}/build)


#
# 检查路径
#
IF (IS_DIRECTORY ${MODULES_SOFTWARE_PATH})

	#
	# 输出信息
	#
	MESSAGE(STATUS "Found " ${PROGRAM_NAME} " Directory : " ${MODULES_SOFTWARE_PATH})

	#
	# 设置路径
	#
	SET(MODULES_SOFTWARE_INCLUDE_PATH		${MODULES_SOFTWARE_PATH}/include)
	SET(MODULES_ARCH_SOFTWARE_INCLUDE_PATH	${MODULES_SOFTWARE_PATH}/arch/x86/include)

ELSE()

	#
	# 输出信息
	#
	MESSAGE(FATAL_ERROR "Not Found " ${PROGRAM_NAME} " Directory : " ${MODULES_SOFTWARE_PATH})

ENDIF()