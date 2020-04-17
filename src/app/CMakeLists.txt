find_package(Boost 1.65 COMPONENTS program_options filesystem REQUIRED COMPONENTS date_time)

add_executable(medord medord.cpp)
add_executable(medor medor.cpp)

target_link_libraries(medord LINK_PUBLIC dbus model ${Boost_LIBRARIES})
target_link_libraries(medor LINK_PUBLIC dbus ${Boost_LIBRARIES})
