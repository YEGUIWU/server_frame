#include <iostream>
#include "../src/log.h" 
#include <iostream>
//#include <ctime>
int main()
{
    ygw::log::Logger::ptr logger(new ygw::log::Logger);
    logger->AddAppender(ygw::log::LogAppender::ptr(new ygw::log::StdoutLogAppender));
    ygw::log::FileLogAppender::ptr file_appender(new ygw::log::FileLogAppender("./log.txt"));
    file_appender->SetLevel(ygw::log::LogLevel::kError);
    logger->AddAppender(file_appender);

    ygw::log::LogFormatter::ptr fmt(new ygw::log::LogFormatter("%d%T%m%n"));
    file_appender->SetFormatter(fmt);

    YGW_LOG_INFO(logger) << "Hello world";
    YGW_LOG_ERROR(logger) << "test error";
    YGW_LOG_FMT_ERROR(logger, "test fmt error %s", "aa");
    YGW_LOG_FMT_INFO(logger, "test fmt info %s", "aa");

    auto l = ygw::log::LogManager::GetInstance()->GetLogger("xx");
    YGW_LOG_INFO(l) << "xxx";
    return 0;
}
