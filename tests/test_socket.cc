/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: tests/test_socket.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-16
 *  Description: 
 * ====================================================
 */

#include <server_frame/iomanager.h>
#include <server_frame/log.h>
#include <server_frame/socket.h>

static ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();

void test_sock()
{
    ygw::socket::IPAddress::ptr addr = ygw::socket::Address::LookupAnyIPAddress("www.baidu.com"); // 获取百度的地址
    if (addr)
    {
        YGW_LOG_INFO(g_logger) << "get address: " << addr->ToString();
    }
    else
    {
        YGW_LOG_ERROR(g_logger) << "get address fail";
    }

    ygw::socket::Socket::ptr sock = ygw::socket::Socket::CreateTCP(addr);
    addr->SetPort(80);
    if (!sock->Connect(addr))
    {
        YGW_LOG_ERROR(g_logger) << "connect " << addr->ToString() << "fail";
        return;
    }
    else
    {
        YGW_LOG_INFO(g_logger) << "connect " << addr->ToString() << " connected";
    }

    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt =sock->Send(buff, sizeof(buff));
    if (rt <= 0)
    {
        YGW_LOG_INFO(g_logger) << "send fail rt=" << rt;
        return;
    }

    std::string buffer;
    buffer.resize(4096);
    rt = sock->Recv(&buffer[0], buffer.size());

    if (rt <= 0)
    {
        YGW_LOG_INFO(g_logger) << "Recv fail rt=" << rt;
        return;
    }

    buffer.resize(rt);
    YGW_LOG_INFO(g_logger) << buffer;
}

int main(int argc, char** argv)
{
    ygw::scheduler::IOManager iom;
    iom.Schedule(test_sock);
    return 0;
}
