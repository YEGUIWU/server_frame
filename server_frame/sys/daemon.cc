/**
 * @file server_frame/daemon.cc
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-10-28
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */

#include <server_frame/log.h>
#include <server_frame/config.h>

#include <cstring>
#include <ctime>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
//#include <time.h>

#include "daemon.h"

namespace ygw {

    namespace sys {

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        static ygw::config::ConfigVar<uint32_t>::ptr g_daemon_restart_interval = 
            ygw::config::Config::Lookup("daemon.restart_interval", (uint32_t)5, "daemon restart interval");

        std::string ProcessInfo::ToString() const 
        {
            std::stringstream ss;
            ss << "[ProcessInfo parent_id=" << parent_id
                << " main_id=" << main_id
                << " parent_start_time=" << ygw::util::TimeUtil::Time2Str(parent_start_time)
                << " main_start_time=" << ygw::util::TimeUtil::Time2Str(main_start_time)
                << " restart_count=" << restart_count << "]";
            return ss.str();
        }

        static int RealStart(int argc, char** argv,
                std::function<int(int argc, char** argv)> main_cb) 
        {
            ProcessInfoManager::GetInstance()->main_id = getpid();
            ProcessInfoManager::GetInstance()->main_start_time = time(0);
            return main_cb(argc, argv);
        }
        
        static int RealDaemon(int argc, char** argv,
                std::function<int(int argc, char** argv)> main_cb) 
        {
            if (daemon(1,0) == -1)
            {
                YGW_LOG_ERROR(g_logger) << "daemon error" << " errno=" << errno << " errstr=" << strerror(errno);
                return -1;
            }
            ProcessInfoManager::GetInstance()->parent_id = getpid();
            ProcessInfoManager::GetInstance()->parent_start_time = time(0);
            while(true) 
            {
                pid_t pid = fork();
                if(pid == 0) //子进程
                {
                    ProcessInfoManager::GetInstance()->main_id = getpid();
                    ProcessInfoManager::GetInstance()->main_start_time  = time(0);
                    YGW_LOG_INFO(g_logger) << "process start pid=" << getpid();
                    return RealStart(argc, argv, main_cb);
                } 
                else if(pid < 0) // error 
                {
                    YGW_LOG_ERROR(g_logger) << "fork fail return=" << pid
                        << " errno=" << errno << " errstr=" << strerror(errno);
                    return -1;
                } 
                else //父进程
                {
                    int status = 0;
                    waitpid(pid, &status, 0);
                    if(status)  // 异常退出
                    {
                        if(status == 9)  // 被砍了
                        {
                            YGW_LOG_INFO(g_logger) << "killed";
                            break;
                        } 
                        else 
                        {
                            YGW_LOG_ERROR(g_logger) << "child crash pid=" << pid
                                << " status=" << status;
                        }
                    } 
                    else  // status == 0 means 正常退出
                    {
                        YGW_LOG_INFO(g_logger) << "child finished pid=" << pid;
                        break;
                    }
                    ProcessInfoManager::GetInstance()->restart_count += 1; // 记录子进程重启次数
                    sleep(g_daemon_restart_interval->GetValue()); // 等待上一个子进程资源释放
                }
            }
            return 0;
        }

        int StartDaemon(int argc, char** argv
                , std::function<int(int argc, char** argv)> main_cb
                , bool is_daemon) 
        {
            if(!is_daemon) 
            {
                ProcessInfoManager::GetInstance()->parent_id = getpid();
                ProcessInfoManager::GetInstance()->parent_start_time = time(0);
                return RealStart(argc, argv, main_cb);
            }
            return RealDaemon(argc, argv, main_cb);
        }

    }

}
