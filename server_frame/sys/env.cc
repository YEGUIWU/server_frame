/**
 * @file server_frame/sys/env.cc
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-10-28
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <iomanip>

#include <server_frame/config.h>
#include <server_frame/log.h>
#include "env.h"

namespace ygw {

    namespace sys {

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        bool Env::Init(int argc, char** argv) 
        {
            //char link[1024] = {0};
            //char path[1024] = {0};
            char link[256] = {0};
            char path[256] = {0};
            sprintf(link, "/proc/%d/exe", getpid());
            if (readlink(link, path, sizeof(path)) == -1)
            {
                YGW_LOG_ERROR(g_logger) << "readlink error" << " errno=" << errno << " strerror=" << strerror(errno);
                return -1;
            }
            // /path/xxx/exe
            exe_ = path;

            auto pos = exe_.find_last_of("/");
            cwd_ = exe_.substr(0, pos) + "/";

            program_ = argv[0];
            // -config /path/to/config -file xxxx -d
            const char* now_key = nullptr;
            for (int i = 1; i < argc; ++i) 
            {
                if (argv[i][0] == '-') // key
                {
                    if (strlen(argv[i]) > 1) 
                    {
                        if (now_key) 
                        {
                            Add(now_key, "");
                        }
                        now_key = argv[i] + 1;
                    } 
                    else 
                    {
                        YGW_LOG_ERROR(g_logger) << "invalid arg idx=" << i
                            << " val=" << argv[i];
                        return false;
                    }
                } 
                else  // value
                {
                    if (now_key)
                    {
                        Add(now_key, argv[i]);
                        now_key = nullptr;
                    } 
                    else 
                    {
                        YGW_LOG_ERROR(g_logger) << "invalid arg idx=" << i
                            << " val=" << argv[i];
                        return false;
                    }
                }
            }
            if (now_key) 
            {
                Add(now_key, "");
            }
            return true;
        }

        void Env::Add(const std::string& key, const std::string& val)
        {
            RWMutexType::WriteLock lock(mutex_);
            args_[key] = val;
        }

        bool Env::Has(const std::string& key) 
        {
            RWMutexType::ReadLock lock(mutex_);
            auto it = args_.find(key);
            return it != args_.end();
        }

        void Env::Del(const std::string& key) 
        {
            RWMutexType::WriteLock lock(mutex_);
            args_.erase(key);
        }

        std::string Env::Get(const std::string& key, const std::string& default_value) 
        {
            RWMutexType::ReadLock lock(mutex_);
            auto it = args_.find(key);
            return it != args_.end() ? it->second : default_value;
        }

        void Env::AddHelp(const std::string& key, const std::string& desc) 
        {
            RemoveHelp(key);
            RWMutexType::WriteLock lock(mutex_);
            helps_.push_back(std::make_pair(key, desc));
        }

        void Env::RemoveHelp(const std::string& key) 
        {
            RWMutexType::WriteLock lock(mutex_);
            for (auto it = helps_.begin();
                    it != helps_.end();) 
            {
                if (it->first == key)
                {
                    it = helps_.erase(it);
                } 
                else
                {
                    ++it;
                }
            }
        }

        void Env::PrintHelp() 
        {
            RWMutexType::ReadLock lock(mutex_);
            std::cout << "Usage: " << program_ << " [options]" << std::endl;
            for (auto& i : helps_) 
            {
                std::cout << std::setw(5) << "-" << i.first << " : " << i.second << std::endl;
            }
        }

        bool Env::SetEnv(const std::string& key, const std::string& val) 
        {
            return !setenv(key.c_str(), val.c_str(), 1);
        }

        std::string Env::GetEnv(const std::string& key, const std::string& default_value) 
        {
            const char* v = getenv(key.c_str());
            if (v == nullptr) 
            {
                return default_value;
            }
            return v;
        }

        std::string Env::GetAbsolutePath(const std::string& path) const 
        {
            if (path.empty()) 
            {
                return "/";
            }
            if (path[0] == '/') 
            {
                return path;
            }
            return cwd_ + path;
        }

        std::string Env::GetAbsoluteWorkPath(const std::string& path) const 
        {
            if (path.empty()) 
            {
                return "/";
            }
            if (path[0] == '/') 
            {
                return path;
            }
            static ygw::config::ConfigVar<std::string>::ptr s_server_work_path =
                ygw::config::Config::Lookup<std::string>("server.work_path");
            return s_server_work_path->GetValue() + "/" + path;
        }

        std::string Env::GetConfigPath() 
        {
            return GetAbsolutePath(Get("c", "conf"));
        }


    } // namespace sys

} // namespace ygw
