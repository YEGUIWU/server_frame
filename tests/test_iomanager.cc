#include <server_frame/iomanager.h>
#include <server_frame/log.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <memory>

ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();


void test_fiber()
{
    YGW_LOG_INFO(g_logger) << "test_fiber";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "39.156.69.79", &addr.sin_addr.s_addr);

    if (!connect(sock, (const sockaddr*)&addr, sizeof(addr)))
    {
        
    }
    else if (errno == EINPROGRESS)
    {
        YGW_LOG_INFO(g_logger) << "add event errno = " << errno << " " << strerror(errno);
        ygw::scheduler::IOManager::GetThis()->AddEvent(sock, ygw::scheduler::IOManager::Event::kRead, [](){
            YGW_LOG_INFO(g_logger) << "read callback";
        });
        ygw::scheduler::IOManager::GetThis()->AddEvent(sock, ygw::scheduler::IOManager::Event::kWrite, [sock](){
            YGW_LOG_INFO(g_logger) << "write callback";
            ygw::scheduler::IOManager::GetThis()->CancelEvent(sock, ygw::scheduler::IOManager::kRead);
            close(sock);
        });
    }
    else
    {
        YGW_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }

}
static ygw::timer::Timer::ptr s_timer;
void test_timer()
{
    ygw::scheduler::IOManager iom(2);
    s_timer = iom.AddTimer(1000, [](){
        static int i = 0;
        YGW_LOG_INFO(g_logger) << "Hello Timer";
        //std::cout << "Hello Timer" << std::endl;
        if (++i == 3)
        {
            s_timer->Reset(2000, true);
        }
        else if (i == 5)
        {
            s_timer->Cancel();
        }
    }, true);
}

void test1()
{
    ygw::scheduler::IOManager iom;
    iom.Schedule(&test_fiber);
}

int main()
{
    test1();
    //test_timer();

    return 0;
}
