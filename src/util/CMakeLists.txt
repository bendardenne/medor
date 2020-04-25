file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/util/*.h")

find_package(Boost 1.65 REQUIRED log_setup log date_time)

add_library(util STATIC time.cpp database.cpp ${HEADER_LIST})
target_include_directories(util PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(util ${Boost_LIBRARIES})
