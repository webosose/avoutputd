# @@@LICENSE
#
#      Copyright (c) 2016-17 LG Electronics, Inc.
#
# Confidential computer software. Valid license from LG required for
# possession, use or copying. Consistent with FAR 12.211 and 12.212,
# Commercial Computer Software, Computer Software Documentation, and
# Technical Data for Commercial Items are licensed to the U.S. Government
# under vendor's standard commercial license.
#
# LICENSE@@@

cmake_minimum_required(VERSION 2.8.7)

project(avoutputd CXX)
include(webOS/webOS)
webos_modules_init(1 0 0 QUALIFIER RC7)
webos_component(1 0 0)

webos_add_compiler_flags(ALL -std=c++11 -Wall -Wextra -fno-permissive)
#disable specific warnings
webos_add_compiler_flags(ALL -Wno-unused-parameter -Wno-deprecated-declarations -Wno-type-limits -Wno-comment)
#promote specific warnings to errors
webos_add_compiler_flags(ALL -Werror=return-type  -Werror=reorder -Werror=uninitialized)

include(FindPkgConfig)

pkg_check_modules(GLIB2 REQUIRED glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${GLIB2_CFLAGS})

pkg_check_modules(LUNASERVICE2 REQUIRED luna-service2++)
include_directories(${LUNASERVICE2_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${LUNASERVICE2_CFLAGS})

pkg_check_modules(LS2HELPERS REQUIRED ls2-helpers)
include_directories(${LS2HELPERS_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${LS2HELPERS_CFLAGS})

pkg_check_modules(PBNJSON_CXX REQUIRED pbnjson_cpp)
include_directories(${PBNJSON_CXX_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${PBNJSON_CXX_CFLAGS_OTHER})

pkg_check_modules(PMLOG REQUIRED PmLogLib)
include_directories(${PMLOG_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${PMLOG_CFLAGS_OTHER})

pkg_check_modules(AVAL_API REQUIRED avoutput-adaptation-layer-api)
include_directories(${AVAL_API_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${AVAL_API_CFLAGS_OTHER})

pkg_check_modules(AVAL_IMPL REQUIRED aval-impl)
include_directories(${AVAL_IMPL_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${AVAL_IMPL_CFLAGS_OTHER})


file(GLOB SOURCES src/*.cpp src/video/*.cpp src/audio/*.cpp src/settings/*.cpp src/systemproperty/*.cpp)

set(START_ON_SIGNAL "lginit_app-ready and stopped dbus")
if(${WEBOS_TARGET_DISTRO} STREQUAL "apollo")
  set(START_ON_SIGNAL "ls-hubd_public-ready")
endif(${WEBOS_TARGET_DISTRO} STREQUAL "apollo")

webos_add_linker_options(ALL --no-undefined)

#add local include directories, accessible only by double quotes include.
webos_add_compiler_flags(ALL -iquote ${CMAKE_CURRENT_BINARY_DIR}/Configured/files/conf)
webos_add_compiler_flags(ALL -iquote ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(avoutputd ${SOURCES})

target_link_libraries(avoutputd
        ${AVAL_IMPL_LDFLAGS}
        ${GLIB2_LDFLAGS}
        ${LUNASERVICE2_LDFLAGS}
        ${LS2HELPERS_LDFLAGS}
        ${PBNJSON_CXX_LDFLAGS}
        ${PMLOG_LDFLAGS}
        rt
        pthread)

webos_build_daemon()
webos_build_system_bus_files()

add_subdirectory(test)
