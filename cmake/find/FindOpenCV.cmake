#
# 设置包名
#
SET(PROGRAM_NAME OpenCV)


#
# 设置路径
#
SET(OPENCV_SOFTWARE_PATH /home/software/opencv/opencv-3.2.0)


#
# 检查路径
#
IF (IS_DIRECTORY ${OPENCV_SOFTWARE_PATH})

	#
	# 输出信息
	#
	MESSAGE(STATUS "Found " ${PROGRAM_NAME} " Directory : " ${OPENCV_SOFTWARE_PATH})

	#
	# 设置路径
	#
	SET(OPENCV_SOFTWARE_LIBRARY_PATH		${OPENCV_SOFTWARE_PATH}/lib)
	SET(OPENCV_SOFTWARE_INCLUDE_PATH		${OPENCV_SOFTWARE_PATH}/include)

ELSE()

	#
	# 输出信息
	#
	MESSAGE(FATAL_ERROR "Not Found " ${PROGRAM_NAME} " Directory : " ${OPENCV_SOFTWARE_PATH})

ENDIF()
