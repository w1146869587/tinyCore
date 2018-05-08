#
# 设置包名
#
SET(PROGRAM_NAME FFmpeg)


#
# 设置路径
#
SET(FFMPEG_SOFTWARE_PATH /home/software/ffmpeg/ffmpeg-3.3.4)


#
# 检查路径
#
IF (IS_DIRECTORY ${FFMPEG_SOFTWARE_PATH})

	#
	# 输出信息
	#
	MESSAGE(STATUS "Found " ${PROGRAM_NAME} " Directory : " ${FFMPEG_SOFTWARE_PATH})

	#
	# 设置路径
	#
	SET(FFMPEG_SOFTWARE_LIBRARY_PATH		${FFMPEG_SOFTWARE_PATH}/lib)
	SET(FFMPEG_SOFTWARE_INCLUDE_PATH		${FFMPEG_SOFTWARE_PATH}/include)

ELSE()

	#
	# 输出信息
	#
	MESSAGE(FATAL_ERROR "Not Found " ${PROGRAM_NAME} " Directory : " ${FFMPEG_SOFTWARE_PATH})

ENDIF()
