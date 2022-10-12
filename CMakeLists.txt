cmake_minimum_required(VERSION 3.18.4)
project(libnecrolog LANGUAGES CXX)
include(CTest)

set(CMAKE_SHARED_LIBRARY_PREFIX "") # we don't want CMake to prepend "lib" to our libraries, we prefer adding that ourselves

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(NECROLOG_WARNING_FLAGS "-Wall -Wextra -pedantic -Wshadow -Wcast-align -Wunused -Wpedantic -Wconversion -Wsign-conversion -Wmisleading-indentation -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(NECROLOG_WARNING_FLAGS "-Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast ${NECROLOG_WARNING_FLAGS}")
    endif()

    set(CMAKE_CXX_FLAGS "${NECROLOG_WARNING_FLAGS} ${CMAKE_CXX_FLAGS}")
endif()

add_library(libnecrolog SHARED libnecrolog/necrolog.cpp)

target_include_directories(libnecrolog PUBLIC libnecrolog)
target_include_directories(libnecrolog PUBLIC include)
target_compile_definitions(libnecrolog PRIVATE NECROLOG_BUILD_DLL)

if(BUILD_TESTING)
    add_executable(necro-test tests/necro-test.cpp)
    add_test(NAME necro-test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/necro-test)
    target_include_directories(necro-test PRIVATE libnecrolog)
    target_link_libraries(necro-test PRIVATE libnecrolog)
endif()

install(TARGETS libnecrolog)
install(DIRECTORY libnecrolog TYPE INCLUDE FILES_MATCHING PATTERN "*.h")
