file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/storage/*.hpp")
add_library(storage STATIC ${HEADER_LIST} ProjectStore.cpp ActivityStore.cpp VcsStore.cpp Database.cpp)
target_link_libraries(storage model util ${CONAN_LIBS})
target_include_directories(storage PUBLIC ${CMAKE_SOURCE_DIR}/include)
