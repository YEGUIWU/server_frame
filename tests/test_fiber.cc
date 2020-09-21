#include <server_frame/base/fiber.h>
#include <server_frame/log.h>

ygw::log::Logger::ptr g_logger =  YGW_LOG_ROOT();


void run_in_fiber()
{
    YGW_LOG_INFO(g_logger) << "run_in_fiber begin";
    ygw::scheduler::Fiber::YieldToHold();
    YGW_LOG_INFO(g_logger) << "run_in_fiber end";
    ygw::scheduler::Fiber::YieldToHold();
}

void test_fiber()
{
    YGW_LOG_INFO(g_logger) << "main begin - 1";
    {
        ygw::scheduler::Fiber::GetThis();
        YGW_LOG_INFO(g_logger) << "main begin";
        ygw::scheduler::Fiber::ptr fiber(new ygw::scheduler::Fiber(run_in_fiber));
        fiber->SwapIn();
        YGW_LOG_INFO(g_logger) << "main after SwapIn";
        fiber->SwapIn();
        YGW_LOG_INFO(g_logger) << "main after end";
        fiber->SwapIn();
    }
    YGW_LOG_INFO(g_logger) << "main after end - 2";

}

int main()
{
    ygw::thread::Thread::SetName("main");
    std::vector<ygw::thread::Thread::ptr> ths;
    for (int i = 0; i < 3; ++i)
    {
        ths.push_back(ygw::thread::Thread::ptr(
                    new ygw::thread::Thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for (auto &i : ths)
    {
        i->Join();
    }
    return 0;
}
