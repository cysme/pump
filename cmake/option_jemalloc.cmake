#
# Define macros: 
#   JEMALLOC_CXX 
#   JEMALLOC_MANGLE 
#
# Export variables:
#   JEMALLOC_LIBRARY - jemalloc library link path
#

IF(WITH_JEMALLOC)
	ADD_DEFINITIONS("-DJEMALLOC_CXX")
	ADD_DEFINITIONS("-DJEMALLOC_MANGLE")
	
	IF(BUILD_DEBUG)
		FIND_LIBRARY(JEMALLOC_LIBRARY NAMES jemallocd libjemallocd)
	ELSE()
		FIND_LIBRARY(JEMALLOC_LIBRARY NAMES jemalloc libjemalloc)
	ENDIF()
	MESSAGE(STATUS "Jemalloc library: ${JEMALLOC_LIBRARY}")
ENDIF()
