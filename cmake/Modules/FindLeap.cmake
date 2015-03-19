# Locate Leap Motion SDK
#
# This module defines
# LEAP_LIBRARY
# LEAP_FOUND, if false, do not try to link to leap
# LEAP_INCLUDE_DIR, where to find the headers
#
# $LEAPSDK_DIR is an environment variable that may be
# used to locate the Leap Motion SDK directory
#
# Created by Johannes Scholz. 

FIND_PATH(LEAP_INCLUDE_DIR Leap.h
    $ENV{LEAPSDK_DIR}/include
    $ENV{LEAPSDK_DIR}
	${CMAKE_INSTALL_PREFIX}/include
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /opt/include
    /usr/freeware/include
)

FIND_LIBRARY(LEAP_LIBRARY 
    NAMES Leap
    PATHS
    $ENV{LEAPSDK_DIR}/lib
    $ENV{LEAPSDK_DIR}
	${CMAKE_INSTALL_PREFIX}/lib
    /usr/lib/Leap # default leap sdk install dir
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/lib
    /usr/lib
    /opt/lib
    /usr/freeware/lib64
)

FIND_LIBRARY(LEAP_LIBRARY_DEBUG 
    NAMES Leap${CMAKE_DEBUG_POSTFIX}
    PATHS
    $ENV{LEAPSDK_DIR}/lib
    $ENV{LEAPSDK_DIR}
	${CMAKE_INSTALL_PREFIX}/lib
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/lib
    /usr/lib
    /opt/lib
    /usr/freeware/lib64
)

SET(LEAP_FOUND "NO")
IF(LEAP_LIBRARY AND LEAP_INCLUDE_DIR)
    SET(LEAP_FOUND "YES")
ENDIF(LEAP_LIBRARY AND LEAP_INCLUDE_DIR)