/**
 * @file examples/http_server.cc
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-10-27
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */

#include <server_frame/http/http_server.h>
#include <server_frame/log.h>
#include <server_frame/config.h>
#include <iostream>
static ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();

void Run()
{
    ygw::http::HttpServer::ptr server(new ygw::http::HttpServer);
    ygw::socket::Address::ptr addr = ygw::socket::Address::LookupAnyIPAddress("0.0.0.0:8020");
    YAML::Node conf = YAML::LoadFile("./bin/conf/http_server.yml");
    ygw::config::Config::LoadFromYaml(conf);
    ygw::config::ConfigVar<ygw::http::HttpServerConfig>::ptr cfg = 
        ygw::config::Config::Lookup<ygw::http::HttpServerConfig>("http_server.default");
    if (cfg)
    {
        server->SetName(cfg->GetValue().GetName());
        server->SetRoot(cfg->GetValue().GetRoot());
    }
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
    iom.Schedule(Run);
    return 0;
}
