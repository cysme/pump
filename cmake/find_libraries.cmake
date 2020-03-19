#
# Export variables:
#	GNUTLS_LIBRARY - gnutls library link path
#   JEMALLOC_LIBRARY - jemalloc library link path
#
# Export functions:
#	build_pump_library()
#

IF(WIN32)
	SET(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES};.a")
	SET(CMAKE_STATIC_LIBRARY_SUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX};.a")
	SET(CMAKE_SHARED_LIBRARY_PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX};.a")
ENDIF()

SET(CMAKE_PREFIX_PATH ${ROOT_DIR}/lib)

IF(WITH_GNUTLS)
	FIND_LIBRARY(GNUTLS_LIBRARY NAMES gnutls libgnutls libgnutls.dll)
	MESSAGE(STATUS "GnuTls library: ${GNUTLS_LIBRARY}")
ENDIF()

IF(WITH_JEMALLOC)
	IF(BUILD_DEBUG)
		FIND_LIBRARY(JEMALLOC_LIBRARY NAMES jemallocd libjemallocd)
	ELSE()
		FIND_LIBRARY(JEMALLOC_LIBRARY NAMES jemalloc libjemalloc)
	ENDIF()
	MESSAGE(STATUS "Jemalloc library: ${JEMALLOC_LIBRARY}")
ENDIF()