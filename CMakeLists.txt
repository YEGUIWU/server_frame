cmake_minimum_required(VERSION 3.0)
project(server_frame)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O2 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

include_directories(include) # 包含目录

set (LIB_SRC
    src/log.cc
    src/util.cc
    src/config.cc
    )

add_library(src SHARED ${LIB_SRC})

# add_executable(tests tests/test.cc)
# add_definitions(tests src)
# target_link_libraries(tests src)

add_executable(tests tests/test_config.cc)
add_definitions(tests src)
target_link_libraries(tests src -lyaml-cpp)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
