#include <cassert>

#include <server_frame/log.h>
#include <server_frame/util.h>
#include <server_frame/macro.h>

ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();

void test_assert() 
{
    //YGW_LOG_INFO(g_logger) << ygw::util::BacktraceToString(10, 2, "   ");
    YGW_ASSERT(false);
}

int main()
{
    test_assert();

    return 0;
}
