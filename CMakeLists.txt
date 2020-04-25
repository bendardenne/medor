cmake_minimum_required(VERSION 3.15)
project(medor)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.65 REQUIRED program_options filesystem log_setup log date_time)
find_package(PkgConfig)
pkg_check_modules(GIOMM giomm-2.4)

add_subdirectory(src/util)
add_subdirectory(src/dbus)
add_subdirectory(src/model)
add_subdirectory(src/storage)

add_executable(medor src/app/medor.cpp)
add_executable(medord src/app/medord.cpp)

link_directories(${GIOMM_LIBRARY_DIRS})
include_directories(${GIOMM_INCLUDE_DIRS})
target_include_directories(medord PUBLIC ${CMAKE_SOURCE_DIR}/include ${GIOMM_INCLUDE_DIRS})

target_link_libraries(medord LINK_PUBLIC dbus ${Boost_LIBRARIES} ${GIOMM_LIBRARIES})
target_link_libraries(medor LINK_PUBLIC dbus ${Boost_LIBRARIES})

install(TARGETS medord DESTINATION bin)
install(TARGETS medor DESTINATION bin)