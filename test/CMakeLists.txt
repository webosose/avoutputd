# @@@LICENSE
#
# Copyright (c) 2016 LG Electronics, Inc.
#
# Unless otherwise specified or set forth in the NOTICE file, all content,
# including all source code files and documentation files in this repository are:
# Confidential computer software. Valid license from LG required for
# possession, use or copying.
#
# LICENSE@@@

message(STATUS "BUILDING tests")

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION ${WEBOS_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}/test
        FILES_MATCHING PATTERN "integration/*"
                       PATTERN "luna/*.py")
