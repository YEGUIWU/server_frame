/**
 * @file tests/test_daemon.cc
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-10-28
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#include <server_frame/sys/daemon.h>
#include <server_frame/iomanager.h>
#include <server_frame/log.h>

static ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();

int ServerMain(int argc, char* argv[])
{
    ygw::scheduler::IOManager iom(1);
    ygw::timer::Timer::ptr timer;
    timer = iom.AddTimer(1000, [&](){
        YGW_LOG_INFO(g_logger) << "Ontimer";
        static int count  = 0;
        if (++count > 10)
        {
            timer->Cancel();
        }
    }, true);
    return 0;
}

int main(int argc, char* argv[])
{
    std::cout << getpid() << std::endl;
    return ygw::sys::StartDaemon(argc, argv, ServerMain, argc != 1);
}
