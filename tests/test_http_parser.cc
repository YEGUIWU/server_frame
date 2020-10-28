/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: tests/test_http_parser.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-19
 *  Description: 测试http_parser
 * ====================================================
 */
#include <server_frame/http/http_parser.h>
#include <server_frame/log.h>

static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

static char test_request_data[] = 
    "GET / HTTP/1.1\r\n"
    "Host: www.baidu.com\r\n"
    "Content-Length: 10\r\n\r\n"
    "1234567890";

void test_request()
{
    ygw::http::HttpRequestParser parser;
    std::string tmp = test_request_data;
    size_t s = parser.Execute(&tmp[0], tmp.size());
    YGW_LOG_INFO(g_logger) << "Execute rt=" << s 
        << " has_error=" << parser.HasError()
        << " is_finished=" << parser.IsFinished()
        << " total=" << tmp.size()
        << " content_length=" << parser.GetContentLength();
    YGW_LOG_INFO(g_logger) << parser.GetData()->ToString();
    YGW_LOG_INFO(g_logger) << tmp;
}
static char test_response_data[] = 
    "HTTP/1.0 200 OK\r\n"
    "Accept-Ranges: bytes\r\n"
    "Cache-Control: no-cache\r\n"
    "Content-Length: 81\r\n"
    "Content-Type: text/html\r\n"
    "Date: Sat, 19 Sep 2020 03:07:29 GMT\r\n"
    "Pragma: no-cache\r\n"
    "Server: BWS/1.1\r\n\r\n"
    "<html>\r\n"
    "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
    "</html>\r\n";

void test_response()
{
    ygw::http::HttpResponseParser parser;
    std::string tmp = test_response_data;
    size_t s = parser.Execute(&tmp[0], tmp.size(), true);
    YGW_LOG_INFO(g_logger) << "Execute rt=" << s
        << " has_error=" << parser.HasError()
        << " is_finished=" << parser.IsFinished()
        << " total=" << tmp.size()
        << " content_length=" << parser.GetContentLength()
        << " tmp[s]=" << tmp[s];
    YGW_LOG_INFO(g_logger) << parser.GetData()->ToString();
}
int main(int argc, char** argv)
{
    test_request();
    test_response();
    return 0;
}
