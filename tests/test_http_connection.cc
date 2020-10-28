/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: tests/test_http_connection.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-23
 *  Description: 
 * ====================================================
 */

#include <iostream>
#include <server_frame/http/http_connection.h>
#include <server_frame/log.h>

static ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();

void run()
{
    ygw::socket::Address::ptr addr = ygw::socket::Address::LookupAnyIPAddress("www.baidu.com:80");
    if (!addr)
    {
        YGW_LOG_INFO(g_logger) << "get addr error";
        return;
    }

    ygw::socket::Socket::ptr sock = ygw::socket::Socket::CreateTCP(addr);
    bool rt = sock->Connect(addr);
    if (!rt)
    {
        YGW_LOG_INFO(g_logger) << "Connect:" << *addr << " failed";
        return;
    }

    ygw::http::HttpConnection::ptr conn(new ygw::http::HttpConnection(sock));
    ygw::http::HttpRequest::ptr req(new ygw::http::HttpRequest);
    req->SetHeader("host", "www.baidu.com");
    YGW_LOG_INFO(g_logger) << "req: " << std::endl
        << *req;

    conn->SendRequest(req);
    auto rsp = conn->RecvResponse();

    if (!rsp)
    {
        YGW_LOG_INFO(g_logger) << "recv response error";
        return;
    }
    YGW_LOG_INFO(g_logger) << "rsp: " << std::endl
        << *rsp;
}

int main(int argc, char** argv)
{
    ygw::scheduler::IOManager iom(2);
    iom.Schedule(run);
    return 0;
}
