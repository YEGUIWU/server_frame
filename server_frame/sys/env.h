/**
 * @file server_frame/sys/env.h
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-10-28
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGW_SYS_ENV_H__
#define __YGW_SYS_ENV_H__

#include <map>
#include <vector>

#include <server_frame/singleton.h>
#include <server_frame/base/thread.h>

namespace ygw {

    namespace sys {
        
        /**
         * @brief 
         */
        class Env {
        public:
            using RWMutexType = ygw::thread::RWMutex;

            /**
             * @brief 初始化并解析参数
             *
             * @param[in] argc 参数个数
             * @param[in] argv[] 参数数组
             *
             * @return 是否初始化成功
             */
            bool Init(int argc, char* argv[]);

            /**
             * @brief 添加环境变量
             *
             * @param[in] key
             * @param[in] val
             */
            void Add(const std::string& key, const std::string& val);

            /**
             * @brief 判断是否存在指定环境变量
             *
             * @param[in] key
             *
             * @return 
             */
            bool Has(const std::string& key);

            /**
             * @brief 根据指定key删除变量
             *
             * @param[in] key
             */
            void Del(const std::string& key);

            /**
             * @brief 根据key获取对应的环境变量的值
             *
             * @param[in] key
             * @param[in] default_value
             *
             * @return 
             */
            std::string Get(const std::string& key, const std::string& default_value = "");

            /**
             * @brief 
             *
             * @param[in] key
             * @param[in] desc
             */
            void AddHelp(const std::string& key, const std::string& desc);

            /**
             * @brief 根据key删除指定帮助
             *
             * @param[in] key 
             */
            void RemoveHelp(const std::string& key);

            /**
             * @brief 打印帮助信息
             */
            void PrintHelp();

            /**
             * @brief 
             *
             * @return 
             */
            const std::string& GetExe() const { return exe_;}

            /**
             * @brief 
             *
             * @return 
             */
            const std::string& GetCwd() const { return cwd_;}

            /**
             * @brief 
             *
             * @param[in] key
             * @param[in] val
             *
             * @return 
             */
            bool SetEnv(const std::string& key, const std::string& val);

            /**
             * @brief 
             *
             * @param[in] key
             * @param[in] default_value
             *
             * @return 
             */
            std::string GetEnv(const std::string& key, const std::string& default_value = "");

            /**
             * @brief 
             *
             * @param[in] path
             *
             * @return 
             */
            std::string GetAbsolutePath(const std::string& path) const;

            /**
             * @brief
             *
             * @param[in] path
             *
             * @return 
             */
            std::string GetAbsoluteWorkPath(const std::string& path) const;

            /**
             * @brief 
             *
             * @return 
             */
            std::string GetConfigPath();
        private:
            ///
            RWMutexType mutex_;
            ///
            std::map<std::string, std::string> args_;
            ///
            std::vector<std::pair<std::string, std::string>> helps_;
            ///
            std::string program_;
            ///
            std::string exe_;
            ///
            std::string cwd_;
        }; // class Env

        using EnvManager = ygw::mode::Singleton<Env>;
        

    } // namespace sys

} // namespace ygw

#endif //__YGW_SYS_ENV_H__
