#include <unistd.h>
#include <server_frame/base/scheduler.h>
#include <server_frame/log.h>

ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();
//ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");
void test_fiber()
{
    YGW_LOG_INFO(g_logger) << "test in fiber";
    sleep(1);
    ygw::scheduler::Scheduler::GetThis()->Schedule(&test_fiber);
}
int main()
{
    YGW_LOG_INFO(g_logger) << "main";
    //ygw::scheduler::Scheduler sc(3, false, "test");
    ygw::scheduler::Scheduler sc(2);
    sc.Start();
    //YGW_LOG_INFO(g_logger) << "schedule";
    sc.Schedule(&test_fiber);
    sc.Stop();
    //YGW_LOG_INFO(g_logger) << "over";
    return 0;
}
