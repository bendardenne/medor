file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/dbus/*.h")

add_library(dbus STATIC CLI.cpp Tracker.cpp VcsHinter.cpp Notifier.cpp ${HEADER_LIST})
target_include_directories(dbus PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(dbus PRIVATE util model storage vcs ${CONAN_LIBS})