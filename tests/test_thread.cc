#include <thread.h>
#include <config.h>
#include <vector>
#include <string>
#include <iostream>

ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();
int g_count = 0;
void func1()
{
    YGW_LOG_INFO(g_logger) << "name: " << ygw::thread::Thread::GetThisName()
                           << " this.name: " << ygw::thread::Thread::GetThisPtr()->GetName()
                           << " this.id: " << ygw::thread::Thread::GetThisPtr()->GetId();

    for (int i = 0; i < 100000000; ++i)
    {
        ++g_count;
    }
}

void func2()
{
    while (true)
    {
        YGW_LOG_INFO(g_logger) << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    }
}

void func3()
{
    while (true)
    {
        YGW_LOG_INFO(g_logger) << "==========================================";
    }
}

int main()
{
    YGW_LOG_INFO(g_logger) << "thread test begin";
    //YAML::Node root = YAML::LoadFile("./bin/conf/test2.yml");
    ygw::config::Config::LoadFromYamlFile("./bin/conf/test2.yml");
    std::vector<ygw::thread::Thread::ptr> ths;
    //for (int i = 0; i < 5; ++i)
    //{
    //    ths.emplace_back(new ygw::thread::Thread(&func1, "name_" + std::to_string(i)));
    //}

    int n = 2;
    for (int i = 0; i < n; ++i)
    {
        ths.emplace_back(new ygw::thread::Thread(&func2, "name_" + std::to_string(i * 2)));
        ths.emplace_back(new ygw::thread::Thread(&func3, "name_" + std::to_string(i * 2 + 1)));
    }


    for (int i = 0; i < n; ++i)
    {
        ths[i]->Join();
    }

    YGW_LOG_INFO(g_logger) << "thread test end";
    YGW_LOG_INFO(g_logger) << "count = " << g_count;
    return 0;
}
