find_package(sdbus-c++ REQUIRED)

## TODO move to cmake file ?
find_package(PkgConfig QUIET)
pkg_check_modules(LIBNOTIFY QUIET libnotify)

find_path(LIBNOTIFY_INCLUDE_DIRS
        NAMES notify.h
        HINTS ${LIBNOTIFY_INCLUDEDIR}
        ${LIBNOTIFY_INCLUDE_DIRS}
        PATH_SUFFIXES libnotify
        )

find_library(LIBNOTIFY_LIBRARIES
        NAMES notify
        HINTS ${LIBNOTIFY_LIBDIR}
        ${LIBNOTIFY_LIBRARY_DIRS}
        )

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibNotify REQUIRED_VARS LIBNOTIFY_INCLUDE_DIRS LIBNOTIFY_LIBRARIES
        VERSION_VAR   LIBNOTIFY_VERSION)

mark_as_advanced(
        LIBNOTIFY_INCLUDE_DIRS
        LIBNOTIFY_LIBRARIES
)


file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/dbus/*.h")

add_library(dbus STATIC CLI.cpp Tracker.cpp ${HEADER_LIST})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_LIB_PREFIX "lib")
target_include_directories(dbus PUBLIC ${CMAKE_SOURCE_DIR}/include ${LIBNOTIFY_INCLUDE_DIRS})

target_link_libraries(dbus PRIVATE util model storage SDBusCpp::sdbus-c++ ${LIBNOTIFY_LIBRARIES})