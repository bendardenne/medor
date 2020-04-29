find_package(SQLite3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)


include_directories(${SQLite3_INCLUDE_DIRS})

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/storage/*.h")
add_library(storage STATIC ${HEADER_LIST} ProjectStore.cpp ActivityStore.cpp VcsStore.cpp)

target_link_libraries(storage model util ${SQLite3_LIBRARIES} ${Boost_LIBRARIES})
target_include_directories(storage PUBLIC ${CMAKE_SOURCE_DIR}/include)

