#include <server_frame/hook.h>
#include <server_frame/iomanager.h>
#include <server_frame/log.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();

void test_sleep()
{
    ygw::scheduler::IOManager iom(1);
    iom.Schedule([](){
        sleep(2);
        YGW_LOG_INFO(g_logger) << "sleep 2"; 
    });

    iom.Schedule([](){
        sleep(3);
        YGW_LOG_INFO(g_logger) << "sleep 3"; 
    });
    YGW_LOG_INFO(g_logger) << "sleep 3";
}

void test_sockcet()
{

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    //inet_pton(AF_INET, "39.156.69.79", &addr.sin_addr.s_addr);
    inet_pton(AF_INET, "14.215.177.39", &addr.sin_addr.s_addr);
    //inet_pton(AF_INET, "118.193.98.63", &addr.sin_addr.s_addr);
    YGW_LOG_INFO(g_logger) << "begin connect"; 
    int rt = connect(sock, (const sockaddr*)&addr, sizeof(addr));
    YGW_LOG_INFO(g_logger) << "connect rt = " << rt << " errno=" << errno;

    if (rt)
    {
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    YGW_LOG_INFO(g_logger) << "send rt = " << rt << " errno = " << errno;

    if (rt <= 0)
    {
        return;
    }

    std::string buff;
    buff.resize(4096);
    rt = recv(sock, &buff[0], buff.size(), 0);
    YGW_LOG_INFO(g_logger) << "recv rt = " << rt << " errno = " << errno;

    if (rt <= 0)
    {
        return;
    }

    //buff.resize(rt);
    YGW_LOG_INFO(g_logger) << buff;
}

int main()
{
    //test_sleep();
    ygw::scheduler::IOManager iom;
    iom.Schedule(test_sockcet);
    //test_sockcet();
    return 0;
}
