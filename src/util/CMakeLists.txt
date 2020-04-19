file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/util/*.h")

add_library(util STATIC time.cpp ${HEADER_LIST})

target_include_directories(util PUBLIC ${CMAKE_SOURCE_DIR}/include)

