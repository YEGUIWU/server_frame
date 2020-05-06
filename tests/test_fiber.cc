#include <server_frame/fiber.h>
#include <server_frame/log.h>

ygw::log::Logger::ptr g_logger =  YGW_LOG_ROOT();


void run_in_fiber()
{
    YGW_LOG_INFO(g_logger) << "run_in_fiber begin";
    ygw::thread::Fiber::YieldToHold();
    YGW_LOG_INFO(g_logger) << "run_in_fiber end";
    ygw::thread::Fiber::YieldToHold();
}

int main()
{
    ygw::thread::Thread::SetName("main");
    YGW_LOG_INFO(g_logger) << "main begin - 1";
    {
        ygw::thread::Fiber::GetThis();
        YGW_LOG_INFO(g_logger) << "main begin";
        ygw::thread::Fiber::ptr fiber(new ygw::thread::Fiber(run_in_fiber));
        fiber->SwapIn();
        YGW_LOG_INFO(g_logger) << "main after SwapIn";
        fiber->SwapIn();
        YGW_LOG_INFO(g_logger) << "main after end";
        fiber->SwapIn();
    }
    YGW_LOG_INFO(g_logger) << "main after end - 2";
    return 0;
}
