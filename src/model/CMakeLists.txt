file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/model/*.h")
add_library(model STATIC Activity.cpp ${HEADER_LIST})
target_include_directories(model PUBLIC ${CMAKE_SOURCE_DIR}/include)
