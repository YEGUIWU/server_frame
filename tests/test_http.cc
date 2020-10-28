/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: tests/test_http.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-18
 *  Description: 测试Http模块
 * ====================================================
 */

#include <server_frame/http/http.h>
#include <server_frame/log.h>


void test_request()
{
    using namespace ygw::http;
    HttpRequest::ptr req(new HttpRequest);
    req->SetHeader("host", "www.baidu.com");
    req->SetBody("hello ygw");
    req->Dump(std::cout) << std::endl;
}

void test_response()
{
    using namespace ygw::http;
    HttpResponse::ptr rsp(new HttpResponse);
    rsp->SetHeader("X-X", "guiwu");
    rsp->SetBody("hello guiwu");
    rsp->SetStatus((HttpStatus)400);
    rsp->SetClose(false);
    rsp->Dump(std::cout) << std::endl;
}

int main(int argc, char** argv)
{
    test_request();
    test_response();


    return 0;
}
