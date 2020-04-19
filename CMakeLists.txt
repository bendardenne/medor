cmake_minimum_required(VERSION 3.15)
project(medor)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src/util)
add_subdirectory(src/dbus)
add_subdirectory(src/model)
add_subdirectory(src/storage)

# TODO REQUIRED ?
find_package(Boost 1.65 COMPONENTS program_options filesystem REQUIRED COMPONENTS date_time)

add_executable(medord src/app/medord.cpp)
add_executable(medor src/app/medor.cpp)

target_link_libraries(medord LINK_PUBLIC dbus ${Boost_LIBRARIES})
target_link_libraries(medor LINK_PUBLIC dbus ${Boost_LIBRARIES})

install(TARGETS medord DESTINATION bin)
install(TARGETS medor DESTINATION bin)