cmake_minimum_required(VERSION 3.15)
project(medor)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src/dbus)
add_subdirectory(src/model)
add_subdirectory(src/app)
add_subdirectory(src/storage)

install(TARGETS medord DESTINATION bin)
install(TARGETS medor DESTINATION bin)