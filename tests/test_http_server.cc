/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: tests/test_http_server.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-20
 *  Description: 
 * ====================================================
 */

#include <server_frame/http/http_server.h>
#include <server_frame/log.h>
#include <server_frame/config.h>
#include <iostream>
static ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();

void run()
{
    ygw::http::HttpServer::ptr server(new ygw::http::HttpServer);
    ygw::socket::Address::ptr addr = ygw::socket::Address::LookupAnyIPAddress("0.0.0.0:8020");
    while (!server->Bind(addr))
    {
        sleep(2);
    }
    auto sd = server->GetServletDispatch();

    sd->AddServlet("/ygw/hello", [](ygw::http::HttpRequest::ptr req,
                ygw::http::HttpResponse::ptr rsp,
                ygw::http::HttpSession::ptr session){
            std::cout << req->GetFragment() << std::endl;
            rsp->SetBody(req->ToString());
            return 0;
    });
    // 通配要在最后才加
    sd->AddGlobServlet("/ygw/*", [](ygw::http::HttpRequest::ptr req,
                ygw::http::HttpResponse::ptr rsp,
                ygw::http::HttpSession::ptr session){
            rsp->SetBody("Glob:\r\n" + req->ToString());
            return 0;
    });
    server->Start();
}
int main(int argc, char** argv)
{
    ygw::scheduler::IOManager iom(2);
    iom.Schedule(run);
    return 0;
}
