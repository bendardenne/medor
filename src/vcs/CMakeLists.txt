file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/vcs/*.hpp")
add_library(vcs STATIC HgClient.cpp HgProtocol.cpp IVcsClient.cpp GitClient.cpp ${HEADER_LIST})
target_include_directories(vcs PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(vcs ${CONAN_LIBS})
