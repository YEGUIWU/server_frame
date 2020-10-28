/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: examples/echo_server.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-19
 *  Description: echo_server
 * ====================================================
 */

#include <server_frame/tcp_server.h>
#include <server_frame/log.h>
#include <server_frame/iomanager.h>
#include <server_frame/bytearray.h>

static ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();

class EchoServer : public ygw::tcp::TcpServer
{
public:
    EchoServer(int type);
    void HandleClient(ygw::socket::Socket::ptr client);
private:
    // 
    int type_ = 0;
};

EchoServer::EchoServer(int type)
    : type_(type)
{
} 

void EchoServer::HandleClient(ygw::socket::Socket::ptr client)
{
    YGW_LOG_INFO(g_logger) << "HandleClient" << *client;
    ygw::container::ByteArray::ptr ba(new ygw::container::ByteArray);
    int rt;
    while (true)
    {
        ba->Clear();
        std::vector<iovec> iovs;
        ba->GetWriteBuffers(iovs, 1024);

        rt = client->Recv(&iovs[0], iovs.size());
        if (rt == 0)
        {
            YGW_LOG_INFO(g_logger) << "client close: " << *client;
            break;
        }
        else if (rt < 0)
        {
            YGW_LOG_INFO(g_logger) << "client error rt=" << rt
                << " errno=" << errno << " errstr" << strerror(errno);
            break;
        }

        ba->SetPosition(ba->GetPosition() + rt);
        // continue to write
        // ...

        ba->SetPosition(0);
        //
        if (type_ == 1) // text
        {
            std::cout << ba->ToString();// std::endl;
        }
        else
        {
            std::cout << ba->ToHexString();// std::endl;
        }
        std::cout.flush();
    }
}

int type = 1;
void run()
{
    YGW_LOG_INFO(g_logger) << "server type=" << type;
    EchoServer::ptr es(new EchoServer(type));
    auto addr = ygw::socket::Address::LookupAny("0.0.0.0:8020");
    while (!es->Bind(addr))
    {
        sleep(2);
    }
    es->Start();
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        YGW_LOG_INFO(g_logger) << "used as[" << argv[0] << " -t] or [" << argv[0] << "-b]";
        return 0;
    }

    if (!strcmp(argv[1], "-b"))
    {
        type = 2;
    }

    ygw::scheduler::IOManager iom(2);
    iom.Schedule(run);

    return 0;
}
