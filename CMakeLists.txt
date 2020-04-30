cmake_minimum_required(VERSION 3.12)
project(medor VERSION 0.0.1 DESCRIPTION "Time tracking tool")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(src/util)
add_subdirectory(src/model)
add_subdirectory(src/storage)
add_subdirectory(src/vcs)
add_subdirectory(src/dbus)

add_executable(medor src/app/medor.cpp)
add_executable(medord src/app/medord.cpp)

target_include_directories(medord PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(medord LINK_PUBLIC dbus ${CONAN_LIBS})
target_link_libraries(medor LINK_PUBLIC dbus ${CONAN_LIBS})

install(TARGETS medord DESTINATION bin)
install(TARGETS medor DESTINATION bin)