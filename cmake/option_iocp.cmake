#
# Define macros: 
#   USE_IOCP 
#

IF(WIN32 AND WITH_IOCP)
	ADD_DEFINITIONS("-DUSE_IOCP")
ENDIF()