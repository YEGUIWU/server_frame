cmake_minimum_required(VERSION 3.0)
project(server_frame)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

include_directories(.)
# include_directories(include) # 包含目录
# include_directories(server_frame) # 包含目录

set (LIB_SRC
    server_frame/log.cc
    server_frame/util.cc
    server_frame/config.cc
    server_frame/thread.cc
    server_frame/mutex.cc
    server_frame/fiber.cc
    )

add_library(server_frame SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(server_frame)

set(LIBS
    server_frame 
    yaml-cpp
    pthread
    )
# add_executable(tests tests/test.cc)
# add_definitions(tests src)
# target_link_libraries(tests src)
ygw_add_executable(test_log "tests/test_log.cc" server_frame "${LIBS}")
ygw_add_executable(test_config "tests/test_config.cc" server_frame "${LIBS}")
ygw_add_executable(test_thread "tests/test_thread.cc" server_frame "${LIBS}")
ygw_add_executable(test_util "tests/test_util.cc" server_frame "${LIBS}")
ygw_add_executable(test_fiber "tests/test_fiber.cc" server_frame "${LIBS}")
# add_executable(tests tests/test_config.cc)
# add_definitions(tests src)
# force_redefine_file_macro_for_sources(tests)
# target_link_libraries(tests src -lyaml-cpp)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
