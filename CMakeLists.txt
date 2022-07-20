cmake_minimum_required(VERSION 3.18.4)
project(libnecrolog LANGUAGES CXX)
include(CTest)

set(CMAKE_STATIC_LIBRARY_PREFIX "") # we don't want CMake to prepend "lib" to our libraries, we prefer adding that ourselves

add_library(libnecrolog SHARED libnecrolog/necrolog.cpp)

target_include_directories(libnecrolog PUBLIC libnecrolog)
target_include_directories(libnecrolog PUBLIC include)

if(BUILD_TESTING)
    add_executable(necro-test tests/necro-test.cpp)
    add_test(NAME necro-test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/necro-test)
    target_include_directories(necro-test PRIVATE libnecrolog)
    target_link_libraries(necro-test PRIVATE libnecrolog)
endif()
