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
    server_frame/address.cc
    server_frame/base/fd_manager.cc
    server_frame/base/fiber.cc
    server_frame/base/mutex.cc
    server_frame/base/scheduler.cc
    server_frame/base/thread.cc
    server_frame/base/timer.cc
    server_frame/bytearray.cc
    server_frame/config.cc
    server_frame/hook.cc
    server_frame/http/http.cc
    server_frame/http/http_connection.cc
    server_frame/http/http_parser.cc
    server_frame/http/http_server.cc
    server_frame/http/http_session.cc
    server_frame/http/parser/http11_parser.rl.cc
    server_frame/http/parser/httpclient_parser.rl.cc
    server_frame/http/servlet.cc
    server_frame/http/uri.rl.cc
    server_frame/iomanager.cc
    server_frame/log.cc
    server_frame/socket.cc
    server_frame/stream/socket_stream.cc
    server_frame/stream/stream.cc
    server_frame/stream/zlib_stream.cc
    server_frame/sys/daemon.cc
    server_frame/sys/env.cc
    server_frame/tcp_server.cc
    server_frame/util.cc
    server_frame/util/crypto_util.cc
    server_frame/util/hash_util.cc
    server_frame/util/json_util.cc
    # server_frame/http/servlet/config_servlet.cc
    # server_frame/http/servlet/status_servlet.cc
    )

add_library(server_frame SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(server_frame)

set(LIBS
    server_frame 
    yaml-cpp
    jsoncpp
    pthread
    ssl
    z
    dl
    protobuf
    )
# add_executable(tests tests/test.cc)
# add_definitions(tests src)
# target_link_libraries(tests src)

# add_executable(tests tests/test_config.cc)
# add_definitions(tests src)
# force_redefine_file_macro_for_sources(tests)
# target_link_libraries(tests src -lyaml-cpp)

# test
ygw_add_executable(test_log "tests/test_log.cc" server_frame "${LIBS}")
ygw_add_executable(test_config "tests/test_config.cc" server_frame "${LIBS}")
ygw_add_executable(test_thread "tests/test_thread.cc" server_frame "${LIBS}")
ygw_add_executable(test_util "tests/test_util.cc" server_frame "${LIBS}")
ygw_add_executable(test_fiber "tests/test_fiber.cc" server_frame "${LIBS}")
ygw_add_executable(test_scheduler "tests/test_scheduler.cc" server_frame "${LIBS}")
ygw_add_executable(test_iomanager "tests/test_iomanager.cc" server_frame "${LIBS}")
ygw_add_executable(test_hook "tests/test_hook.cc" server_frame "${LIBS}")
ygw_add_executable(test_address "tests/test_address.cc" server_frame "${LIBS}")
ygw_add_executable(test_socket "tests/test_socket.cc" server_frame "${LIBS}")
ygw_add_executable(test_bytearray "tests/test_bytearray.cc" server_frame "${LIBS}")
ygw_add_executable(test_http "tests/test_http.cc" server_frame "${LIBS}")
ygw_add_executable(test_http_parser "tests/test_http_parser.cc" server_frame "${LIBS}")
ygw_add_executable(test_tcp_server "tests/test_tcp_server.cc" server_frame "${LIBS}")
ygw_add_executable(test_http_server "tests/test_http_server.cc" server_frame "${LIBS}")
ygw_add_executable(test_http_connection "tests/test_http_connection.cc" server_frame "${LIBS}")
ygw_add_executable(test_daemon "tests/test_daemon.cc" server_frame "${LIBS}")
ygw_add_executable(test_env "tests/test_env.cc" server_frame "${LIBS}")
# examples
ygw_add_executable(echo_server "examples/echo_server.cc" server_frame "${LIBS}")
ygw_add_executable(http_server "examples/http_server.cc" server_frame "${LIBS}")


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
