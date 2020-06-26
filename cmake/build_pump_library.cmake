#
# Export functions:
#	build_pump_library()
#
# Run:
#   build_pump_library()
#

MACRO(build_pump_library)
	set_complie_flags(${LIB_COMPILE_FLAGS})

	IF(JEMALLOC_LIBRARY)
		FILE(GLOB COM_SOURCES ${ROOT_DIR}/*.cpp)
	ENDIF()
	FILE(GLOB_RECURSE LIB_SOURCES ${ROOT_DIR}/src/*.cpp)
	FILE(GLOB_RECURSE LIB_HEADERS ${ROOT_DIR}/include/pump/*.h)
	SET(LIB_ALL_FILES ${LIB_SOURCES} ${LIB_HEADERS} ${COM_SOURCES})

	IF(WIN32)
		FOREACH(FILE_NAME ${LIB_ALL_FILES})
			IF("${FILE_NAME}" MATCHES ".cpp")
				FILE(RELATIVE_PATH REL_FILE_NAME "${ROOT_DIR}/src" ${FILE_NAME})
			ELSE("${FILE_NAME}" MATCHES ".h")
				FILE(RELATIVE_PATH REL_FILE_NAME "${ROOT_DIR}/include/pump" ${FILE_NAME})
			ENDIF()
			GET_FILENAME_COMPONENT(REL_FILE_PATH ${REL_FILE_NAME} DIRECTORY)
			IF("${REL_FILE_PATH}" STREQUAL "")
				SOURCE_GROUP("" FILES ${FILE_NAME})
			ELSEIF("${REL_FILE_PATH}" STREQUAL "..")
				SOURCE_GROUP("" FILES ${FILE_NAME})
			ELSE()
				STRING(REPLACE "/" "\\" REL_FILE_PATH "${REL_FILE_PATH}")
				SOURCE_GROUP(${REL_FILE_PATH} FILES ${FILE_NAME})
			ENDIF()
			#MESSAGE(STATUS "file: ${FILE_NAME} path: ${REL_FILE_PATH}")
		ENDFOREACH()
	ENDIF()
	
	ADD_LIBRARY(${LIBRARY_NAME} SHARED ${LIB_ALL_FILES})
	IF(WIN32)
		SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES PREFIX "")
	ENDIF()
	
	IF(BUILD_DEBUG)
		SET(OUTPUT_TARGET_LIB "${LIBRARY_NAME}d")
	ELSE()
		SET(OUTPUT_TARGET_LIB "${LIBRARY_NAME}")
	ENDIF()
	SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_TARGET_LIB})

	IF(WIN32)
		SET(LINK_LIBS "ws2_32.lib")
		IF(GNUTLS_LIBRARY)
			SET(LINK_LIBS "${LINK_LIBS};${GNUTLS_LIBRARY}")
		ENDIF()
		IF(JEMALLOC_LIBRARY)
			SET(LINK_LIBS "${LINK_LIBS};${JEMALLOC_LIBRARY}")
		ENDIF()
	ELSEIF(UNIX)
		SET(LINK_LIBS "pthread")
		IF(GNUTLS_LIBRARY)
			SET(LINK_LIBS "${LINK_LIBS} ${GNUTLS_LIBRARY}")
		ENDIF()
		IF(JEMALLOC_LIBRARY)
			SET(LINK_LIBS "${LINK_LIBS} ${JEMALLOC_LIBRARY}")
		ENDIF()
	ENDIF()
	TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${LINK_LIBS})

	IF(UNIX)
		SET(CMAKE_INSTALL_PREFIX /usr/local)
		FILE(GLOB_RECURSE FUNCTION_HEADERS ${ROOT_DIR}/include/function/*.h)
		FILE(GLOB_RECURSE CCQUEUE_HEADERS ${ROOT_DIR}/include/concurrentqueue/*.h)
		INSTALL(FILES ${FUNCTION_HEADERS} DESTINATION include/function)
		INSTALL(FILES ${CCQUEUE_HEADERS} DESTINATION include/concurrentqueue)
		INSTALL(DIRECTORY ${ROOT_DIR}/include/pump DESTINATION include)
		INSTALL(TARGETS ${LIBRARY_NAME}
				RUNTIME DESTINATION bin
				LIBRARY DESTINATION lib64
				ARCHIVE DESTINATION lib64)
	ENDIF()
ENDMACRO()

build_pump_library()
