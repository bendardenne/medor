file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/hg/*.h")
add_library(hg STATIC HgClient.cpp Protocol.cpp ${HEADER_LIST})
target_include_directories(hg PUBLIC ${CMAKE_SOURCE_DIR}/include)
