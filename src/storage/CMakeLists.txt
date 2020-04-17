find_package(SQLite3)
find_package(Boost 1.65 COMPONENTS filesystem REQUIRED COMPONENTS date_time)

set(CMAKE_CXX_STANDARD 17)

include_directories(${SQLite3_INCLUDE_DIRS})

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/storage/*.hpp")

add_library(storage STATIC ${HEADER_LIST} DB.cpp ../../include/storage/DB.h)
target_link_libraries(storage ${SQLite3_LIBRARIES}  ${Boost_LIBRARIES})

target_include_directories(storage PUBLIC ${CMAKE_SOURCE_DIR}/include)

