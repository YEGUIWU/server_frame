/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: tests/test_tcp_server.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-19
 *  Description: 测试tcp_server
 * ====================================================
 */
#include <server_frame/tcp_server.h>
#include <server_frame/iomanager.h>

ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();

void run()
{
    auto addr = ygw::socket::Address::LookupAny("0.0.0.0:8033");
    //auto addr2 = ygw::socket::UnixAddress::ptr(new ygw::socket::UnixAddress("/tmp/unix_addr"));
    YGW_LOG_INFO(g_logger) << *addr; //<< " - " << *addr2;

    std::vector<ygw::socket::Address::ptr> addrs;
    addrs.push_back(addr);
    //addrs.push_back(addr2);

    ygw::tcp::TcpServer::ptr tcp_server(new ygw::tcp::TcpServer);
    std::vector<ygw::socket::Address::ptr> fails;
    while (!tcp_server->Bind(addrs, fails))
    {
        sleep(2);
    }
    tcp_server->Start();
}

int main(int argc, char** argv)
{
    ygw::scheduler::IOManager iom(2);
    iom.Schedule(run);
     
    return 0;
}
