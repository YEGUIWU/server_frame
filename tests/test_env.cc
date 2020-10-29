/**
 * @file tests/test_env.cc
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-10-28
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#include <unistd.h>
#include <iostream>
#include <fstream>

#include <server_frame/sys/env.h>

struct A {
    A() 
    {
        std::ifstream ifs("/proc/" + std::to_string(getpid()) + "/cmdline", std::ios::binary);
        std::string content;
        content.resize(4096);

        ifs.read(&content[0], content.size());
        content.resize(ifs.gcount());

        for (size_t i = 0; i < content.size(); ++i) 
        {
            std::cout << i << " - " << content[i] << " - " << (int)content[i] << std::endl;
        }
    }
};

A a;

int main(int argc, char*argv[]) 
{
    std::cout << "argc=" << argc << std::endl;
    ygw::sys::EnvManager::GetInstance()->AddHelp("s", "start with the terminal");
    ygw::sys::EnvManager::GetInstance()->AddHelp("d", "run as daemon");
    ygw::sys::EnvManager::GetInstance()->AddHelp("p", "print help");
    if (!ygw::sys::EnvManager::GetInstance()->Init(argc, argv)) 
    {
        ygw::sys::EnvManager::GetInstance()->PrintHelp();
        return 0;
    }

    std::cout << "exe=" << ygw::sys::EnvManager::GetInstance()->GetExe() << std::endl;
    std::cout << "cwd=" << ygw::sys::EnvManager::GetInstance()->GetCwd() << std::endl;

    std::cout << "path=" << ygw::sys::EnvManager::GetInstance()->GetEnv("PATH", "xxx") << std::endl;
    std::cout << "test=" << ygw::sys::EnvManager::GetInstance()->GetEnv("TEST", "") << std::endl;
    std::cout << "set env " << ygw::sys::EnvManager::GetInstance()->SetEnv("TEST", "yy") << std::endl;
    std::cout << "test=" << ygw::sys::EnvManager::GetInstance()->GetEnv("TEST", "") << std::endl;
    if (ygw::sys::EnvManager::GetInstance()->Has("p"))
    {
        ygw::sys::EnvManager::GetInstance()->PrintHelp();
    }
    return 0;
}

